// Matthew Egg and Charles Daigle
// EECE 2560 Project 4 - Part A

#include <iostream>
#include <vector>
#include "d_matrix.h"
#include "d_except.h"
#include <fstream>
using namespace std;

typedef int ValueType;
const int Blank = -1;
const int SquareSize = 3;
const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;

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
    vector<vector<vector<int>>> conflicts;

    void undoChanges(int, int, int, int);
    void updateConflicts(int, int, int, int);
};

/** @brief Constructor for board class
 * @param sqSize The size of the board
 * @return None
*/
board::board(int sqSize) : value(BoardSize + 1, BoardSize + 1), conflicts(BoardSize + 1, vector<vector<int>>(BoardSize + 1, vector<int>(MaxValue + 1, 0)))  // BoardSize+1 by BoardSize+1 matrix, initialized to 0
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
            value[i][j] = Blank;    // Sets all cells to blank
        }

    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
            for (int k = 1; k <= MaxValue; k++)
                conflicts[i][j][k] = 0; // Sets all conflicts to 0
}

/** @brief Initializes the board
 * @param fin The file to read from
 * @return None
*/
void board::initialize(ifstream &fin)
{
    char ch;
    clear();    // Clears the board
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            fin >> ch;  // Reads from file
            if (ch != '.')  // If the character is not a period, set the cell to the character
                setCell(i, j, ch - '0');    // Converts the character to an integer
        }
}

/** @brief Updates the conflicts matrix
 * @param i The row to update
 * @param j The column to update
 * @param val The value to update
 * @param increment The amount to increment by
 * @return None
*/
void board::updateConflicts(int i, int j, int val, int increment)
{
    for (int k = 1; k <= BoardSize; k++)
    {
        conflicts[i][k][val] += increment;  // Increments the row conflicts
        conflicts[k][j][val] += increment;  // Increments the column conflicts
    }

    int rowStart = 3 * ((i - 1) / 3) + 1;   // Calculates the starting row of the square
    int colStart = 3 * ((j - 1) / 3) + 1;   // Calculates the starting column of the square

    for (int row = rowStart; row < rowStart + SquareSize; row++)
        for (int col = colStart; col < colStart + SquareSize; col++)
            conflicts[row][col][val] += increment;  // Increments the square conflicts
}

/** @brief Undoes changes to the conflicts matrix
 * @param i The row to update
 * @param j The column to update
 * @param val The value to update
 * @param increment The amount to increment by
 * @return None
*/
void board::undoChanges(int i, int j, int val, int increment)
{
    updateConflicts(i, j, val, -increment);   // Decrements the conflicts
}

/** @brief Sets a cell to a value
 * @param i The row to update
 * @param j The column to update
 * @param val The value to update
 * @return None
*/
void board::setCell(int i, int j, int val)
{
    int oldVal = value[i][j];
    value[i][j] = val;

    if (oldVal != Blank)    // If the old value is not blank, undo changes
        undoChanges(i, j, oldVal, 1);   // Undoes changes to the conflicts matrix

    if (val != Blank)   // If the new value is not blank, update conflicts
        updateConflicts(i, j, val, 1);  // Updates the conflicts matrix
}

/** @brief Clears a cell
 * @param i The row to update
 * @param j The column to update
 * @return None
*/
void board::clearCell(int i, int j)
{
    int val = value[i][j];  // Gets the value of the cell
    value[i][j] = Blank;    // Sets the cell to blank
    if (val != Blank)
        undoChanges(i, j, val, 1);  // Undoes changes to the conflicts matrix
}

/** @brief Checks if a cell is blank
 * @param i The row to check
 * @param j The column to check
 * @return True if the cell is blank, false otherwise
*/
bool board::isBlank(int i, int j)
{
    return (value[i][j] == Blank);  // Returns true if the cell is blank, false otherwise
}

/** @brief Gets the value of a cell
 * @param i The row to get
 * @param j The column to get
 * @return The value of the cell
*/
ValueType board::getCell(int i, int j)
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j]; // Returns the value of the cell
    else
        throw rangeError("bad value in getCell");   // Throws an error if the cell is out of range
}

/** @brief Checks if the board is solved
 * @param None
 * @return True if the board is solved, false otherwise
*/
bool board::isSolved()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            if (isBlank(i, j))
            {
                return false;   // Returns false if the board is not solved
            }
        }
    }

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
            cout << " -";   // Prints the top of the square
            for (int j = 1; j <= BoardSize; j++)
                cout << "---";  // Prints the top of the square
            cout << "-" << endl;    // Prints the top of the square
        }
        for (int j = 1; j <= BoardSize; j++)
        {
            if ((j - 1) % SquareSize == 0)
                cout << "|";    // Prints the side of the square
            if (!isBlank(i, j))
                cout << " " << getCell(i, j) << " ";
            else
                cout << "   ";
        }
        cout << "|" << endl;    // Prints the side of the square
    }

    cout << " -";   // Prints the bottom of the square
    for (int j = 1; j <= BoardSize; j++)
        cout << "---";  // Prints the bottom of the square
    cout << "-" << endl;    // Prints the bottom of the square
}

/** @brief Prints the conflicts matrix
 * @param None
 * @return None
 * @note This is for debugging purposes
*/
void board::printConflicts()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            cout << "Cell (" << i << ", " << j << "): ";    // Prints the cell
            for (int k = 1; k <= MaxValue; k++)
                cout << conflicts[i][j][k] << " ";  // Prints the conflicts matrix
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