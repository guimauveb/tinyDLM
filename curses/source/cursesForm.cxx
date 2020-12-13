#include "../include/CursesForm.hxx"

/* Form is already declared as a private member variable - the constructor takes the number of fields we want
 * to create in the form */
CursesForm::CursesForm(const size_t& number_of_fields)
{
    for (size_t i = 0; i < number_of_fields; ++i) {
        fields.emplace_back();
    }
    fields.push_back(nullptr);
}


CursesForm::~CursesForm()
{
    clearForm();
}

void CursesForm::setField(size_t field_index, int height, int width, int toprow, int leftcol, int offscreen,
        int nbuffers)
{
    fields.at(field_index) = new_field(height, width, toprow, leftcol, offscreen, nbuffers);
}

/* Init form once we got all infos about fields and their options */
void CursesForm::initForm()
{
    form = new_form(fields.data());
}

void CursesForm::initField(const size_t& number_of_fields) {}

void CursesForm::setFieldBack(size_t field_index, chtype attr)
{
    set_field_back(fields.at(field_index), attr);
}

void CursesForm::fieldOptsOff(const size_t field_index, Field_Options opts)
{
    field_opts_off(fields.at(field_index), opts);
}

void CursesForm::fieldOptsOn(size_t field_index, Field_Options opts)
{
    field_opts_on(fields.at(field_index), opts);
}

/* Doesn't work properly on Linux - use set_form_sub instead */
void CursesForm::setFormWin(std::unique_ptr<CursesWindow>& win)
{
    set_form_win(form, win->getRawPtr());
}

void CursesForm::setFormSubwin(std::unique_ptr<CursesWindow>& win)        
{
    set_form_sub(form, win->getRawPtr());
}

/* TODO - check errors */
int CursesForm::formDriver(int c)
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

void CursesForm::postForm()
{
    post_form(form);
}

std::string CursesForm::getFieldBuffer(size_t field_index)
{
    std::string str = field_buffer(fields.at(field_index), 0);
    return str; 

}
void CursesForm::clearForm()
{
    unpost_form(form);
    free_form(form);

    for (size_t i = 0; i < fields.size(); i++) {
        free_field(fields.at(i));
    }
    fields.clear();
}

/* Save buffer content - useful when resizing the window */
void CursesForm::saveFieldBuffer()
{
    form_driver(form, REQ_FIRST_FIELD);
    for (size_t i = 0; i < fields.size() - 1; ++i) {
        std::string tmp = field_buffer(fields.at(i), 0);
        tmp = trimSpaces(tmp);
        field_buffers.push_back(tmp);
    }
}

void CursesForm::populateField(size_t field_index, const std::string str)
{
    form_driver(form, field_index);
    for (size_t i = 0; i < str.length(); ++i) {
        form_driver(form, str[i]);
    }
}

int CursesForm::curFieldIdx()
{
    return field_index(current_field(form));
}


