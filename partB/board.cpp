// Matthew Egg
// Project 4 - Part B

#include <iostream>
#include <vector>
#include "d_matrix.h"
#include "d_except.h"
#include <fstream>
using namespace std;

typedef int ValueType;
int callCount = 0;
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
    pair<int, int> findEmptyCell();
    bool solveBoard();

private:
    matrix<ValueType> value;
    vector<vector<vector<int>>> conflicts;

    void updateConflicts(int, int, int, int);
    void undoChanges(int, int, int, int);
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

/** @brief Finds an empty cell
 * @param None
 * @return A pair containing the row and column of the empty cell
*/
pair<int, int> board::findEmptyCell()
{
    for (int row = 1; row <= BoardSize; row++)
    {
        for (int col = 1; col <= BoardSize; col++)
        {
            if (isBlank(row, col))
            {
                return make_pair(row, col); // Returns the row and column of the empty cell
            }
        }
    }

    return make_pair(-1, -1);   // Returns -1, -1 if no empty cell is found
}

/** @brief Solves the board
 * @param None
 * @return True if the board is solved, false otherwise
*/
bool board::solveBoard()
{
    callCount++;    // Increments the call count
    pair<int, int> cell = findEmptyCell();  // Finds an empty cell

    if (cell.first == -1 && cell.second == -1)
    {
        return true;    // Returns true if the board is solved
    }

    int i = cell.first;
    int j = cell.second;

    for (int num = MinValue; num <= MaxValue; num++)
    {
        if (conflicts[i][j][num] == 0)  // If there are no conflicts, set the cell to the number
        {
            setCell(i, j, num);

            if (solveBoard())   // If the board is solved, return true
            {
                return true;    // Returns true if the board is solved
            }

            clearCell(i, j);    // Clears the cell
        }
    }

    return false;   // Returns false if the board is not solved
}

int main()
{
    ifstream fin("sudoku.txt");   // Opens the file
    if (!fin)
    {
        cerr << "Cannot open sudoku.txt" << endl;   // Prints an error if the file cannot be opened
        exit(1);    // Exits the program
    }

    try
    {
        board b1(SquareSize);   // Creates a board
        while (fin && fin.peek() != 'Z')    // While the file is not empty and the ending character isn't encountered
        {
            b1.initialize(fin); // Initializes the board
            b1.print(); // Prints the board
            b1.solveBoard();    // Solves the board
            if (b1.isSolved())  // If the board is solved, print the board and the number of recursive calls
            {
                b1.print();
                cout << "Sudoku board is solved!" << endl;
                cout << "Number of recursive calls: " << callCount << endl;
                callCount = 0;
            }
            else    // If the board is not solved, print an error message
            {
                cout << "Sudoku board is not solved." << endl;
            }
        }
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl;  // Prints an error message
        exit(1);    // Exits the program
    }

    return 0;
}