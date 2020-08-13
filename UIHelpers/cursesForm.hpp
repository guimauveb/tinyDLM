#include "cursesWindow.hpp"
#include <form.h>

#include <vector>
#include <iostream>
#include <thread>

class cursesForm {
    public:
        cursesForm(size_t numField);
        ~cursesForm();

        void initForm();
        void setField(int fieldIdx, int height, int width, int toprow, int leftcol, int offscreen, int nbuffers);
        void initField(size_t numFields);
        void setFieldBack(int fieldIdx, chtype attr);
        void fieldOptsOff(int fieldIdx, Field_Options opts);
        void fieldOptsOn(int fieldIdx, Field_Options opts);
        void setFormWin(std::shared_ptr<cursesWindow>& win);
        void setFormSubwin(std::shared_ptr<cursesWindow>& win, int nlines, int ncols, int begy, int begx);        
        void postForm();

        int formDriver(int c);
        std::string getFieldBuffer(int fieldIdx);

        void populateField(int fieldIdx, const std::string str);
        void saveFieldBuffer();
        void restoreFieldBuffer(std::shared_ptr<cursesWindow> win);
        void clearForm();
        /* TODO - move to misc */
        std::string trimSpaces(std::string str);
    private:
        size_t nFields;
        FORM *form = nullptr;
        std::vector<FIELD*> fields;
        std::vector<const std::string> fieldBuffers;
};
