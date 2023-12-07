// Matthew Egg and Charles Daigle
// EECE 2560 Project 4 - Part A

#include <iostream>
#include <vector>
#include "d_matrix.h"
#include "d_except.h"
#include <fstream>
using namespace std;

typedef int ValueType;
const int Blank = -1;   // Indicates that a cell is blank
const int SquareSize = 3;   //  The number of cells in a small square
const int BoardSize = SquareSize * SquareSize;  //  The number of cells in the board
const int MinValue = 1;  //  The minimum digit value that can appear in an empty cell
const int MaxValue = 9; //  The maximum digit value that can appear in an empty cell

class board
{
    public:
        board(int);
        void clear();
        void initialize(ifstream &fin);
        void print();
        void printConflicts();
        bool isBlank(int, int);
        ValueType getCell(int, int);
        void setCell(int, int, int);
        void clearCell(int, int);
        bool isSolved();

    private:
        matrix<ValueType> value;
        matrix<vector<int>> conflicts;

        void updateConflicts(int, int, int);
};

/** @brief Constructor for the board class
 * @param sqSize The size of the board
 * @return None
 */
board::board(int sqSize) : value(BoardSize + 1, BoardSize + 1), conflicts(BoardSize + 1, BoardSize + 1)
{
    clear();
}

/** @brief Clears the board
 * @param None
 * @return None
 */
void board::clear()
{
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            value[i][j] = Blank;
            conflicts[i][j].clear(); // Clear conflicts for each cell
        }
}

/** @brief Initializes the board from a text file
 * @param fin The input file stream
 * @return None
 */
void board::initialize(ifstream &fin)
{
    char ch;
    clear();
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            fin >> ch;
            if (ch != '.')
                setCell(i, j, ch - '0');
        }
}

/** @brief Updates the conflicts for a cell
 * @param i The row of the cell
 * @param j The column of the cell
 * @param val The value of the cell
 * @return None
 */
void board::updateConflicts(int i, int j, int val) {
    // Clear the conflicts for the cell (i, j)
    conflicts[i][j].clear();

    // Iterate over the cells in the same row as (i, j)
    for (int col = 1; col <= BoardSize; ++col) {
        if (value[i][col] != Blank && col != j) {
            conflicts[i][j].push_back(value[i][col]);
        }
    }

    // Iterate over the cells in the same column as (i, j)
    for (int row = 1; row <= BoardSize; ++row) {
        if (value[row][j] != Blank && row != i) {
            conflicts[i][j].push_back(value[row][j]);
        }
    }

    // Determine the square that (i, j) is in
    int squareStartRow = (i - 1) / SquareSize * SquareSize + 1;
    int squareStartCol = (j - 1) / SquareSize * SquareSize + 1;

    // Iterate over the cells in the same square
    for (int row = squareStartRow; row < squareStartRow + SquareSize; ++row) {
        for (int col = squareStartCol; col < squareStartCol + SquareSize; ++col) {
            if (value[row][col] != Blank && !(row == i && col == j)) {
                conflicts[i][j].push_back(value[row][col]);
            }
        }
    }
}

/** @brief Sets the value of a cell
 * @param i The row of the cell
 * @param j The column of the cell
 * @param val The value of the cell
 * @return None
 */
void board::setCell(int i, int j, int val)
{
    value[i][j] = val;
    updateConflicts(i, j, val);
}

/** @brief Clears the value of a cell
 * @param i The row of the cell
 * @param j The column of the cell
 * @return None
 */
void board::clearCell(int i, int j)
{
    value[i][j] = Blank;
    updateConflicts(i, j, Blank);
}

/** @brief Checks if a cell is blank
 * @param i The row of the cell
 * @param j The column of the cell
 * @return True if the cell is blank, false otherwise
 */
bool board::isBlank(int i, int j)
{
    return (value[i][j] == Blank);
}

/** @brief Gets the value of a cell
 * @param i The row of the cell
 * @param j The column of the cell
 * @return The value of the cell
 */
ValueType board::getCell(int i, int j)
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
}

/** @brief Checks if the board is solved
 * @param None
 * @return True if the board is solved, false otherwise
 */
bool board::isSolved() {
    for (int i = 1; i <= BoardSize; i++) {
        for (int j = 1; j <= BoardSize; j++) {
            // If a cell is blank, the board is not solved
            if (isBlank(i, j)) {
                return false;
            }

            // If a cell has conflicts, the board is not solved
            if (!conflicts[i][j].empty()) {
                return false;
            }
        }
    }

    // If we've checked all cells and found no blanks or conflicts, the board is solved
    return true;
}

/** @brief Prints the board
 * @param None
 * @return None
 */
void board::print()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        if ((i - 1) % SquareSize == 0)
        {
            cout << " -";
            for (int j = 1; j <= BoardSize; j++)
                cout << "---";
            cout << "-" << endl;
        }
        for (int j = 1; j <= BoardSize; j++)
        {
            if ((j - 1) % SquareSize == 0)
                cout << "|";
            if (!isBlank(i, j))
                cout << " " << getCell(i, j) << " ";
            else
                cout << "   ";
        }
        cout << "|" << endl;
    }

    cout << " -";
    for (int j = 1; j <= BoardSize; j++)
        cout << "---";
    cout << "-" << endl;
}

/** @brief Prints the conflicts for each cell
 * @param None
 * @return None
 */
void board::printConflicts() {
    for (int i = 1; i <= BoardSize; i++) {
        for (int j = 1; j <= BoardSize; j++) {
            // Print the coordinates of the cell
            cout << "Cell (" << i << ", " << j << "): ";

            // Print the conflicts for the cell
            for (int conflict : conflicts[i][j]) {
                cout << conflict << " ";
            }

            cout << endl;
        }
    }
}

int main()
{
    ifstream fin("sudoku.txt");
    if (!fin)
    {
        cerr << "Cannot open sudoku.txt" << endl;
        exit(1);
    }

    try
    {
        board b1(SquareSize);
        while (fin && fin.peek() != 'Z')
        {
            b1.initialize(fin);
            b1.print();
            b1.printConflicts();
            if (b1.isSolved())
            {
                cout << "Sudoku board is solved!" << endl;
            }
            else
            {
                cout << "Sudoku board is not solved." << endl;
            }
        }
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl;
        exit(1);
    }

    return 0;
}
