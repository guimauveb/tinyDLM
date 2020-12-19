#include "download.hxx"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <string>
#include <memory>
#include <thread>
#include <sstream>
#include <fstream>
#include <mutex>

#include <cmath>
#include <cstring>

class DownloaderCore 
{
    public:
        /* Constructor */
        DownloaderCore(const int& dlid, const std::string& u, const std::string& f, const std::string& sa);

        /* Make sure threads are done executing when calling the destructor */
        ~DownloaderCore(); 

        /* This function will take care of the transfer */
        int dlPerform();

        /* Do the "cURLpp way" by using functors for our Write, Header and Progress callbacks */
        struct WriteCallbackFunctor
        {
            public:
                /* The callback will write to a file on disk */
                WriteCallbackFunctor(std::ofstream * stream)
                    : fStream(stream), writeRound(0)
                {}
                size_t Write(curlpp::Easy *handle, char* ptr, size_t size, size_t nmemb);

            private:
                /* Ban default constructor */
                WriteCallbackFunctor();

                std::ofstream *fStream;
                unsigned writeRound;
        };

        /* TODO - parse the response header -> gives us handy infos about the server / download */
        struct HeaderCallbackFunctor
        {
            public:
                /* Write header in a memory area then parse it to get some useful informations */
                HeaderCallbackFunctor(std::stringstream *sStr)
                    : sStream(sStr)
                {}
                size_t Write(curlpp::Easy *handle, char *ptr, size_t size, size_t nitems);

            private:
                /* Ban default constructor */
                HeaderCallbackFunctor();

                std::stringstream *sStream;
                unsigned writeRound;
        };

        /* Returns number of bytes downloaded / progress / and that's where we signal the curl handle
         * to pause the transfer */
        struct ProgressCallbackFunctor
        {
            public:
                ProgressCallbackFunctor(std::shared_ptr<std::mutex> dmutp, std::shared_ptr<Download> dp, std::shared_ptr<bool> pp, std::shared_ptr<bool> rp)
                    :mutPtr{dmutp}, dlPtr{dp}, pPtr{pp}, rPtr{rp}
                {}
                int Progress(curlpp::Easy *handle, double dltotal, double dlnow, double ultotal, double ulnow);

            private:
                /* Ban default constructor */
                ProgressCallbackFunctor();

                /* Shared pointer pointing to the mutex protecting the download object */
                std::shared_ptr<std::mutex> mutPtr;
                /* Shared pointer pointing to the download struct */
                std::shared_ptr<Download> dlPtr;
                /* Shared pointer pointing to the 'PAUSE' bool */ 
                std::shared_ptr<bool> pPtr;
                /* Shared pointer pointing to the 'RESUME' bool */ 
                std::shared_ptr<bool> rPtr;
                std::chrono::time_point<std::chrono::system_clock> timeNow, timeOld;

                curl_off_t downloaded;
                unsigned writeRound;
                double progress;
                float percent;
                double dlSpeed;
                double dlold;

        };

        /* Returns a reference to the download object pointed by a shared ptr*/
        /* A const ref for performance reasons since main the ui will call it consistanly */
        const std::shared_ptr<Download>& getDownloadInfos();

        //void setLowSpeedLimit(int lowSpeedLim);
        //void setLowSpeedTime(int lowSpeedTim);

        /* Perform the download once we have all necessary info */
        void runThread();
        /* Pause and resume functions */
        void pause();
        void resume();
        void downloadSpeed();

    private:
        /* Ban default constructor */
        DownloaderCore();

        /* Initialise our curlpp handle here so it can be accessed by any function */
        curlpp::Easy cleaner;
        curlpp::Easy request;

        /* Initialize a download struct that will hold all download informations - initialized by the 
         * constructor */
        std::shared_ptr<Download> downloadPtr = std::make_shared<Download>();

        /* Some websites require a valid user agent */
        static const std::string USER_AGENT; 
        const double oneSec = 1000000;
        const double oneMil = 1000000;

        bool ONESECOND = false;
        /* Transfer will occur in a separate thread */
        std::vector<std::thread> ThreadVector;

        /* Create a mutex that will be accessed by the the progress functor and the class instance */
        std::shared_ptr<std::mutex> dlMutPtr = std::make_shared<std::mutex>();
        /* Pointer to a bool signaling to pause the download - accessed by the progress functor and the class
         * instance */
        std::shared_ptr<bool> pausePtr = std::make_shared<bool>(false);
        /* Set to true when download is resumed */
        std::shared_ptr<bool> resumePtr = std::make_shared<bool>(false);

};

