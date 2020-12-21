#include "../include/DownloaderCore.hxx"

/* Initialize the DownloaderCore object with all the need infos to processs the download */
DownloaderCore::DownloaderCore(const int& dlid, const std::string& u, const std::string& f, const std::string& sa)
{
    /* Inetialize our download object- partial initializion but all non set values should be set to 0 or null */
    downloadPtr->id = dlid;
    downloadPtr->url = u;
    downloadPtr->filename = f;
    downloadPtr->saveAs = sa;

    downloadPtr->status = downloadStatus::PENDING;                     
    downloadPtr->progress = 0;
    downloadPtr->downloaded = 0;
    downloadPtr->speed = 0;

    /* TODO - calculate estimated time remaining */
    downloadPtr->eta = "";                       

    /* TODO - parse response header */
    downloadPtr->can_resume = 0;                  /* Set if download can resume */
    downloadPtr->invalid = 0;                    /* Set if error encountered during download */
    downloadPtr->http_code = 0;                   /* http return code */
    downloadPtr->unescaped = "";
    downloadPtr->referer = "";
    downloadPtr->cookie = "";
}

const std::shared_ptr<Download>& DownloaderCore::getDownloadInfos()
{
    std::lock_guard<std::mutex> guard(*dlMutPtr);
    return downloadPtr;
}

/* Make sure our threads are done executing when calling the destructor */
DownloaderCore::~DownloaderCore()
{
    for (size_t i = 0; i < ThreadVector.size(); ++i)
        ThreadVector.at(i).join();
}

/* Compute the current download speed which corresponds to the average speed during the second preceding the call
 * to this function. */
void DownloaderCore::downloadSpeed()
{
    /* Make a copy of the vector and work with the copy to avoid blocking the downloading thread for too
     * long */
    std::vector<std::pair<double, double>> tmp;
    {
        std::lock_guard<std::mutex> guard(*dlMutPtr);
        tmp = downloadPtr->duration_and_dl;
    }
    /* Reverse the values so the most recent ones are at the begining */
    std::reverse(tmp.begin(), tmp.end());
    double totalDur = 0, totalBytes = 0;
    double keptVal = 0;
    /* Iterate from the end to (end - 1sec) */
    for (const auto& el : tmp) {
        totalDur += el.first;
        totalBytes += el.second;
        /* Store the number of elements we had to sum to compute the average */
        keptVal++;
        if (totalDur >= oneSec)
            break;
    }
    /* totalBytes is equal to what has been downloaded in one sec */
    /* We divide by 1M to get the result in MO/s */
    const double currSpeed = totalBytes / oneMil;
    {
        std::lock_guard<std::mutex> guard(*dlMutPtr);
        downloadPtr->speed = currSpeed;
    }
}

/* Use the progress callback to display progress and pause / resume the transfer */
int DownloaderCore::ProgressCallbackFunctor::Progress(curlpp::Easy *handle, double dltotal, double dlnow, double ultotal, double ulnow)
{
    /* TODO - why curl resets dlnow after a resume but finally manage to update it to the total number of
     * bytes downloaded before we paused the transfer ???? */
    writeRound++;
    {
        /* Not sure about this one */
        /* We try to access the mutex but what if another function tries to access it via another copy
         * of this shared_ptr ? */
        std::lock_guard<std::mutex> guard(*mutPtr);
        /* If paused */
        if (*pPtr) {
            curlpp::options::Url url;
            handle->getOpt(url);
            /* Save total bytes downloaded so far so we can resume the transfer later from the last byte */
            dlPtr->downloaded = dlnow;
            dlPtr->status = downloadStatus::PAUSED;
            /* Signal the handle to pause the transfer */
            return CURLPAUSE_RECV;
        }

        dlPtr->status = downloadStatus::DOWNLOADING;
        /* Round to the largest integer value */
        dlPtr->progress = floorf(((dlnow / dltotal) * 100) * 100) / 100;
    }

    /* If it's the first call, set the old values and return */
    if (writeRound == 1) {
        timeOld = std::chrono::system_clock::now();
        dlold = dlnow;
        return 0;
    }
    /* Else update the actual estimated download speed */
    else {
        timeNow = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - timeOld).count();
        /* duration == 40 000 nanoseconds on average */
        /* Save duration and bytes downloaded in a map then compute the average speed during the last
         * second */
        double downloadedAtThisCall = dlnow - dlold;
        {
            std::lock_guard<std::mutex> guard(*mutPtr);
            dlPtr->duration_and_dl.push_back(std::pair<double, double>(duration, downloadedAtThisCall));
        }
    }

    /* Update the old values for the next call */
    timeOld = std::chrono::system_clock::now();
    dlold = dlnow;

    /* If the download is complete (meaning the number of downloaded bytes == the total number of bytes) 
     * change status to completed */
    if ((dlnow == dltotal && dltotal != 0)) {
        std::lock_guard<std::mutex> guard(*mutPtr);
        dlPtr->status = downloadStatus::COMPLETED;
    }
    return 0;
}

/* Our header callback will output the header to a stringstream that we' ll parse for info */
/* TODO - parse response header for infos about the download / server */
size_t DownloaderCore::HeaderCallbackFunctor::Write(curlpp::Easy *handle, char *ptr, size_t size, size_t nitems)
{
    size_t realsize = size * nitems;
    sStream->write(ptr, realsize);
    std::string s = sStream->str();
    //std::cout << s << "\n";
    return realsize;
}

size_t DownloaderCore::WriteCallbackFunctor::Write(curlpp::Easy *handle, char *ptr, size_t size, size_t nmemb)
{
    ++writeRound;

    curlpp::options::Url url;
    handle->getOpt(url);

    // Calculate the real size of the incoming buffer
    size_t realsize = size * nmemb;
    //std::cerr << "write round: " << writeRound << ", url: " << url.getValue() << std::endl;
    fStream->write(ptr, realsize);
    // return the real size of the buffer...
    return realsize;
}

/* Perform the transfer in a distinct thread */
void DownloaderCore::runThread()
{
    ThreadVector.emplace_back(&DownloaderCore::dlPerform, this);
}

/* Set will be used to modify these values after the object has been created */
/* Perform the download once we have all the info we need */
int DownloaderCore::dlPerform()
{
    try {
        /* Initialise our output file stream before anything else so we don't try to download the file
         * if there was an error while opening a file stream */
        std::ofstream outfile;
        /* Initialise our WriteCallback functor that will write to the disk */
        WriteCallbackFunctor fObject(&outfile);
        /* Pass our pointer to the mutex avoiding trouble with our download struct - pass a const ref to
         * the 'PAUSE' bool (since this the Progress function won't modify it) - and a pointer to the 
         * actual download object */
        ProgressCallbackFunctor pObject(dlMutPtr, downloadPtr, pausePtr, resumePtr);
        /* If the transfer is resumed, append the existing file and resume transfer from the last byte
         * downloaded. */
        {
            std::lock_guard<std::mutex> guard(*dlMutPtr);
            if (*resumePtr) {
                outfile.open(downloadPtr->saveAs, std::ofstream::out | std::ofstream::app);
                request.setOpt(new curlpp::options::ResumeFromLarge(downloadPtr->downloaded));
            }
            /* Else create a new file */
            else {
                outfile.open(downloadPtr->saveAs, std::ofstream::out);
            }
        }
        /* Check if the file was correctly opened */
        if (!outfile) {
            /* TODO - throw an actual exception */
            /* TODO - write to log */
            //std::cout << "Error while opening the file for writing. \n";
        }

        /* Make our header callback functor write to stdout */
        std::stringstream strTest;
        HeaderCallbackFunctor oObject(&strTest);

        {
            using namespace std::placeholders;
            /* Set the writer callback to enable cURL to write result on disk */
            curlpp::options::WriteFunction *wfunc = new curlpp::options::WriteFunction(std::bind(&WriteCallbackFunctor::Write, &fObject, &request, _1, _2, _3));
            request.setOpt(wfunc);

            /* Set the header callback to enable cURL to write header to stdout */
            curlpp::options::HeaderFunction *hfunc = new curlpp::options::HeaderFunction(std::bind(&HeaderCallbackFunctor::Write, &oObject, &request, _1, _2, _3));
            request.setOpt(hfunc);

            /* Set the progress callback */
            curlpp::options::ProgressFunction *pfunc = new curlpp::options::ProgressFunction(std::bind(&ProgressCallbackFunctor::Progress, &pObject, &request, _1, _2, _3, _4));
            request.setOpt(pfunc);
        }
        /* Set CURLOPT_NOPROGRESS to 0 to make the progress function called */
        request.setOpt(new curlpp::options::NoProgress(0));

        request.setOpt(new curlpp::options::UserAgent(USER_AGENT.c_str()));
        request.setOpt(new curlpp::options::Url(downloadPtr->url));
        request.setOpt(new curlpp::options::Verbose(false));

        /* Set the time at which the transfer started here then update it in the progress functor */
        request.perform();
    }
    catch (curlpp::LogicError & e) {
        std::ofstream curlpp_ofstr;
        // TODO - Use Log class
        curlpp_ofstr.open("logs/runtime_errors.txt", std::ofstream::out);
        curlpp_ofstr << e.what();
        {
            std::lock_guard<std::mutex> guard(*dlMutPtr);
            downloadPtr->status = downloadStatus::ERROR;
        }
    }
    catch (curlpp::RuntimeError & e) {
        std::ofstream curlpp_ofstr;
        curlpp_ofstr.open("logs/curlpp/curlpp_runtime_error.txt", std::ofstream::out);
        long size = strlen(e.what());
        /* Callback aborted means that the transfer was paused from the progress functor */
        const char cba[] = "Callback aborted";
        if (!strcmp(e.what(), cba)) {
            //do nothing since it means that we paused the transfer
        }
        else {
            curlpp_ofstr.write(e.what(), size);
            {
                std::lock_guard<std::mutex> guard(*dlMutPtr);
                downloadPtr->status = downloadStatus::ERROR;
            }
        }
    }
    return EXIT_SUCCESS;
}

void DownloaderCore::pause()
{
    std::lock_guard<std::mutex> guard(*dlMutPtr);
    *pausePtr = true;
}

void DownloaderCore::resume()
{
    if (downloadPtr->status == downloadStatus::PAUSED) {
        *resumePtr = true;
        *pausePtr = false;
        runThread();
    }
}

const std::string DownloaderCore::USER_AGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.89 Safari/537.36";

