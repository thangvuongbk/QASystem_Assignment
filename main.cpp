/*!**************************************************************************
 *  \No COPYRIGHT
 *  \file   main.cpp
 *  \brief  Entry for the program and class Reverse File Reader
 *  \author thangvv
 *  \date   07/12/22
 *
 *  \brief Requirements Covered:
 *  \n 1)   REQ03a: The constructor for the class will accept one argument that identifies the file to read.
 *  \n 2)   REQ03b: The public method will have this signature: public String readLine() throws IOException
 *  \n 3)   REQ04: The file to read will be set up by the object constructor when the object is created.
 *  \n 4)   REQ05: The readLine method will return a single line from the file, not including a newline character on the end.
 *  \n 5)   REQ05a: The first call to readLine will return the last line from the file.
 *  \n 6)   REQ05b: Each subsequent call to readLine will return the previous line in the file.
 *  \n 7)   REQ05c: The order of the characters in the returned string will be the same as in the file, i.e. the line itself is not reversed.

 *  \note
 *   Revision History:
 *   Date        Author              Description
 *   ------      --------            --------------
 *   07/13/22    thangvv             Initial version

*****************************************************************************/


#include <iostream> // for cin/cout
#include <fstream> // for file handling. std::ifstream
#include <string> // for std:;string
#include <mutex>

using namespace std;

class ReverseFileReader {
    public:
        /**< Constructor */
        ReverseFileReader(std::string path_to_file) : m_path_to_file(path_to_file) {
            m_return_line = "";
            m_length_of_file = 0;
            m_current_pos = 0;
            m_is_final_call = false;
        }
        /** \brief Read the line from the end of file by every call
         *
         * \param No paras
         * \return a null string if file is read fully. else return a corresponding string in the line
         *
         */

        std::string readLine() {
            char get_one_char;
            bool is_break_line_foud = false;
            m_return_line = "";
            /**< Open and check the file whether open file is success or not */
            std::lock_guard<std::mutex> _lock(m_mutex);
            ifstream file(m_path_to_file, ios::in);
            if(file.is_open()) {
                /**< Get the length of file if this is the first line or not final call */
                if(m_current_pos == 0 && m_is_final_call == false) {
                    file.seekg(-1, file.end);
                    m_length_of_file = (int)file.tellg();
                    m_current_pos = m_length_of_file;
                }
                /**< Check whether file has been modified or not */
                if(isFileChange(file)) {
                    cout << "[WARN] File: " << m_path_to_file << " is modified, result could be unexpected\nSTOP READING\n";
                    file.close();
                    exit(1);
                }

                /**< Find a nearest break line from the current position */
                file.seekg(--m_current_pos, file.beg);
                while(!is_break_line_foud) {
                    get_one_char = file.get();
                    /**< if a break line character is found */
                    if(get_one_char == '\n') {
                        is_break_line_foud = true;
                    }
                    /**< if current line is the first line */
                    else if(m_current_pos < 1) {
                       file.seekg(0, file.beg);
                       m_is_final_call = true;
                       is_break_line_foud = true;
                    }
                    else {
                        /**< Set to previous position */
                        file.seekg(--m_current_pos, file.beg);
                    }
                }
                /**< if still having a line to read, return it */
                if(m_current_pos >= 0) {
                  std::getline(file, m_return_line);

                }
                else cout << "[INFO] Nothing to be read\n";
                file.close();
            }
            else {
                cout << "[ERROR] Could not open the file under path: " << m_path_to_file << endl;
            }

            return m_return_line;
        }
    private:
        bool isFileChange(ifstream& in_file){
            bool is_file_change = false;
            in_file.seekg(-1, ios::end);
            if((int)in_file.tellg() != m_length_of_file) {
                cout << "[WARN] Len_Org: " << m_length_of_file << ";Len_Mofified: " << (int)in_file.tellg() << endl;
                is_file_change = true;
            }
            return is_file_change;
        }

    private:
        std::string m_path_to_file;
        std::string m_return_line;
        int m_length_of_file;
        int m_current_pos;
        bool m_is_final_call;
        mutable std::mutex m_mutex;
};

/** \brief main function
 *
 * \param input.txt file is named exactly and default location must be put the same executable file if we are going to run the command from there
 * \param
 * \return true
 *
 */

int main (int argv, char** argc) {
    ReverseFileReader *reserse_file_reader = new ReverseFileReader("input.txt");
    ReverseFileReader *reserse_file_reader_2 = new ReverseFileReader("input.txt");

    /**< client code to test the readLine */
    string get_line = "";
    int run_cout = 0;
    do {
        cout << "run_count: " << ++run_cout << endl;
        get_line = reserse_file_reader->readLine();
        if(get_line != "") cout << get_line << endl;
        cout << "-----------\n";

    } while(get_line != "");



    delete reserse_file_reader;
    return 0;
}
