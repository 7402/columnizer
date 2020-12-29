//
//  columnizer.cpp
//

// clang++ -std=c++11 -stdlib=libc++ -o columnizer columnizer.cpp
//   or
// g++ -std=c++11 -stdlib=libc++ -o columnizer columnizer.cpp

// usage:
//     columnizer [-t N] [-i <path>] [-o <path>]
//        -t N        generate N items of input data for testing, send to stdout
//        -i <path>   input file path
//        -o <path>   output file path

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, const char * argv[]) {
    const int CHARS_PER_LINE = 80;
    const int LINES_PER_PAGE = 64;
    const int INNER_MARGIN = 5;
    const int OUTER_MARGIN = 3;
    const int TOP_MARGIN = 2;
    const int BOTTOM_MARGIN = 2;
    const int COLUMNS = 2;

    vector<string> lines;
    string output("");

    for (int k = 1; k < argc; k++) {
        if (strcmp(argv[k], "-t") == 0 && k + 1 < argc) {
            // generate test input
            for (int i = 0; i < atoi(argv[k + 1]); i++) {
                cout << "Index: " << (i + 1) << endl;
                cout << "First thing: first" << endl;
                cout << "Second thing: second" << endl;
                cout << "Third thing: third" << endl;
                int choice = rand() % 4;
                switch(choice) {
                    case 1:
                        cout << "Fourth thing: fourth" << endl;
                        break;
                    case 2:
                        cout << "Fourth thing: fourth" << endl;
                        cout << "Fifth thing: fifth" << endl;
                        break;
                    case 3:
                        cout << "Fourth thing: fourth" << endl;
                        cout << "Fifth thing: fifth" << endl;
                        cout << "Sixth thing: sixth" << endl;
                        break;
                }

                cout << endl;
                cout << endl;
            }

        } else if (strcmp(argv[k], "-o") == 0 && k + 1 < argc) {
            output = argv[k + 1];
            k++;

        } else if (strcmp(argv[k], "-i") == 0 && k + 1 < argc) {
            ifstream foo(argv[k + 1]);
            cout << "file: " << argv[k + 1] << endl;
            k++;

            if (!foo.is_open()) {
                cout << "cannot open." << endl;

            } else {
                string line;
                while (getline(foo, line) && !foo.eof()) {
                    lines.push_back(line);
                }
            }
        }
    }

    int CHARS_PER_COLUMN = (CHARS_PER_LINE - INNER_MARGIN - OUTER_MARGIN) / COLUMNS;

    for (size_t k = 0; k < lines.size(); k++) {
        if (lines[k].length() > CHARS_PER_COLUMN - 1) {
            cout << "too many columns" << endl;
            exit(1);
        }
    }

    ofstream ofs = ofstream(output.length() == 0 ? "output.txt" : output);

//    cout << lines.size() << " lines" << endl;

    vector<int> block_starts;
    vector<int> block_sizes;

    int block_size = 0;
    int input_line = 0;

    while (input_line < lines.size()) {
        while (input_line < lines.size() && block_size == 0 && lines.at(input_line).length() == 0) {
            input_line++;
        }

        int block_start = input_line;

        while (input_line + block_size < lines.size() && lines.at(input_line + block_size).length() > 0) {
            block_size++;
        }

        input_line += block_size;

        if (block_size > 0) {
            block_starts.push_back(block_start);
            block_sizes.push_back(block_size);
            block_size = 0;
        }
    }

    vector<string> page(LINES_PER_PAGE, "");
    size_t page_line = 0;
    int column = 0;
    bool recto = true;

    for (size_t k = 0; k < block_starts.size(); k++) {
        int block_start = block_starts.at(k);
        int block_size = block_sizes.at(k);

        while (page_line < TOP_MARGIN) {
            page_line++;
        }

        bool finished_block = false;
        while (!finished_block) {
            bool finish_page = false;
            if (block_size > LINES_PER_PAGE - TOP_MARGIN - BOTTOM_MARGIN) {
                // should not happen, exit rather than endless loop
                cout << "block will not fit on page" << endl;
                exit(1);

            } else if (page_line + block_size <= LINES_PER_PAGE - BOTTOM_MARGIN) {
                // room in this column
                for (int j = 0; j < block_size; j++) {
                    string text_row = page[page_line];
                    size_t margin = (recto ? INNER_MARGIN : OUTER_MARGIN) + column * CHARS_PER_COLUMN;
                    size_t padding = margin - text_row.length();
                    if (padding > 0) text_row.append(string(padding, ' '));

                    text_row.append(lines[block_start + j]);

                    page[page_line] = text_row;

                    page_line++;
                }

                page_line++;

                finished_block = true;
                bool last_block = k == block_starts.size() - 1;
                if (last_block) finish_page = true;

            } else if (column < COLUMNS - 1) {
                // start next column
                column++;
                page_line = TOP_MARGIN;

            } else {
                // finish this page, start next
                finish_page = true;
            }

            if (finish_page) {
                for (size_t i = 0; i < LINES_PER_PAGE; i++) {
                    ofs << page[i] << endl;
                    page[i] = "";
                }

                page_line = 0;
                column = 0;
                page_line = TOP_MARGIN;
                recto = !recto;
            }
        }
    }

    return 0;
}
