#include "../../helper/include/userInputCheck.hxx"

#include "CursesWindow.hxx"
#include <form.h>

#include <vector>
#include <thread>

class CursesForm {
    public:
        CursesForm(const size_t& num_of_fields);
        ~CursesForm();

        void initForm();
        void setField(size_t field_index, int height, int width, int toprow, int leftcol, int offscreen, int nbuffers);
        void initField(const size_t& numFields);
        void setFieldBack(size_t field_index, chtype attr);
        void fieldOptsOff(size_t field_index, Field_Options opts);
        void fieldOptsOn(size_t field_index, Field_Options opts);
        void setFormWin(std::unique_ptr<CursesWindow>& win);
        void setFormSubwin(std::unique_ptr<CursesWindow>& win);        
        void postForm();

        int formDriver(int c);
        std::string getFieldBuffer(size_t field_index);

        void populateField(size_t field_index, const std::string str);
        void saveFieldBuffer();
        void clearForm();

        int curFieldIdx();

    private:
        size_t number_of_fields;
        FORM *form = nullptr;
        std::vector<FIELD*> fields;
        std::vector<std::string> fieldBuffers;
};
