#include "cursesForm.hpp"

/* Form is already declared as a private member variable - the constructor takes the number of fields we want
 * to create in the form */
cursesForm::cursesForm(size_t numField)
{
    /* TODO - init field then init and post form */
    fields.resize(2, nullptr);
    nFields = numField;
    for (size_t i = 0; i < numField; ++i)   {
        fields.emplace_back();
    }
    /* Push back a null pointer - not elegant TODO */
    fields.emplace_back();
    fields.at(fields.size() - 1) = nullptr;
}


cursesForm::~cursesForm()
{
    clearForm();
}

void cursesForm::setField(int fieldIdx, int height, int width, int toprow, int leftcol, int offscreen,
        int nbuffers)
{
    fields.at(fieldIdx) = new_field(height, width, toprow, leftcol, offscreen, nbuffers);
}

/* Init form once we got all infos about fields and their options */
void cursesForm::initForm()
{
    form = new_form(fields.data());
}

void cursesForm::initField(size_t numFields)
{

}

void cursesForm::setFieldBack(int fieldIdx, chtype attr)
{
    set_field_back(fields.at(fieldIdx), attr);
}

void cursesForm::fieldOptsOff(int fieldIdx, Field_Options opts)
{
    field_opts_off(fields.at(fieldIdx), opts);
}

void cursesForm::fieldOptsOn(int fieldIdx, Field_Options opts)
{
    field_opts_on(fields.at(fieldIdx), opts);
}

void cursesForm::setFormWin(std::unique_ptr<cursesWindow>& win)
{
    set_form_win(form, win->getRawPtr());
}

void cursesForm::setFormSubwin(std::unique_ptr<cursesWindow>& win, int nlines, int ncols, int begy, int begx)        
{
    set_form_sub(form, derwin(win->getRawPtr(), nlines, ncols, begy, begx));
}

/* TODO - check errors */
int cursesForm::formDriver(int c)
{
    int retCode = form_driver(form, c);
    if (retCode != E_OK) {
        switch (retCode) {
            case E_BAD_ARGUMENT:
                break;
            case E_BAD_STATE:
                break;
            case E_NOT_POSTED:
                break;
            case E_INVALID_FIELD:
                break;
            case E_REQUEST_DENIED:
                break;
            case  E_SYSTEM_ERROR:
                break;
            case  E_UNKNOWN_COMMAND:
                break;
            default:
                break;
        }
    }
    return retCode;
}

void cursesForm::postForm()
{
    post_form(form);
}

std::string cursesForm::getFieldBuffer(int fieldIdx)
{
    /* TODO - catch out of range vector */
    return std::string(field_buffer(fields.at(fieldIdx), 0)); 

}
void cursesForm::clearForm()
{
    unpost_form(form);
    free_form(form);

    for (size_t i = 0; i < fields.size(); i++) {
        free_field(fields.at(i));
    }
    fields.clear();
}

/* Save buffer content - useful when resizing the window */
void cursesForm::saveFieldBuffer()
{
    form_driver(form, REQ_FIRST_FIELD);
    for (size_t i = 0; i < fields.size() - 1; ++i) {
        std::string tmp = field_buffer(fields.at(i), 0);
        tmp = trimSpaces(tmp);
        fieldBuffers.push_back(tmp);
    }
}

/* TODO - import it from helper functions */
std::string cursesForm::trimSpaces(std::string str)
{
    int i = 0, j = str.length() - 1;
    {
        /* Parse until '\0' or a alphanumeric char */
        while (str[i] && (!isalpha(str[i]) && !isdigit(str[i]))) {
            i++;
        }
        /* Move from the end till first char */
        while(!isalpha(str[j]) && !isdigit(str[j])) {
            j--;
        }
    } 
    return str = str.substr(i, j + 1);
}



void cursesForm::populateField(int fieldIdx, const std::string str)
{
    form_driver(form, fieldIdx);
    for (size_t i = 0; i < str.length(); ++i) {
        form_driver(form, str[i]);
    }
}


