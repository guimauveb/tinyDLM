#include "../../helper/include/userInputCheck.hxx"

#include "cursesWindow.hxx"
#include <form.h>

#include <vector>
#include <thread>

class cursesForm {
    public:
        cursesForm(const size_t& numField);
        ~cursesForm();

        void initForm();
        void setField(size_t fieldIdx, int height, int width, int toprow, int leftcol, int offscreen, int nbuffers);
        void initField(const size_t& numFields);
        void setFieldBack(size_t fieldIdx, chtype attr);
        void fieldOptsOff(size_t fieldIdx, Field_Options opts);
        void fieldOptsOn(size_t fieldIdx, Field_Options opts);
        void setFormWin(std::unique_ptr<cursesWindow>& win);
        void setFormSubwin(std::unique_ptr<cursesWindow>& win);        
        void postForm();

        int formDriver(int c);
        std::string getFieldBuffer(size_t fieldIdx);

        void populateField(size_t fieldIdx, const std::string str);
        void saveFieldBuffer();
        void clearForm();

        int curFieldIdx();

    private:
        size_t nFields;
        FORM *form = nullptr;
        std::vector<FIELD*> fields;
        std::vector<std::string> fieldBuffers;
};
