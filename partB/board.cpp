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
    pair<int, int> findEmptyCell();
    bool solveBoard();

private:
    matrix<ValueType> value;
    vector<vector<vector<int>>> conflicts;

    void updateConflicts(int, int, int, int);
    void undoChanges(int, int, int, int);
};

board::board(int sqSize) : value(BoardSize + 1, BoardSize + 1), conflicts(BoardSize + 1, vector<vector<int>>(BoardSize + 1, vector<int>(MaxValue + 1, 0)))
{
    clear();
}

void board::clear()
{
    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
        {
            value[i][j] = Blank;
        }

    for (int i = 1; i <= BoardSize; i++)
        for (int j = 1; j <= BoardSize; j++)
            for (int k = 1; k <= MaxValue; k++)
                conflicts[i][j][k] = 0;
}

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

void board::updateConflicts(int i, int j, int val, int increment)
{
    for (int k = 1; k <= BoardSize; k++)
    {
        conflicts[i][k][val] += increment;
        conflicts[k][j][val] += increment;
    }

    int rowStart = 3 * ((i - 1) / 3) + 1;
    int colStart = 3 * ((j - 1) / 3) + 1;

    for (int row = rowStart; row < rowStart + SquareSize; row++)
        for (int col = colStart; col < colStart + SquareSize; col++)
            conflicts[row][col][val] += increment;
}

void board::undoChanges(int i, int j, int val, int increment)
{
    updateConflicts(i, j, val, -increment);
}

void board::setCell(int i, int j, int val)
{
    int oldVal = value[i][j];
    value[i][j] = val;

    if (oldVal != Blank)
        undoChanges(i, j, oldVal, 1);

    if (val != Blank)
        updateConflicts(i, j, val, 1);
}

void board::clearCell(int i, int j)
{
    int val = value[i][j];
    value[i][j] = Blank;
    if (val != Blank)
        undoChanges(i, j, val, 1);
}

bool board::isBlank(int i, int j)
{
    return (value[i][j] == Blank);
}

ValueType board::getCell(int i, int j)
{
    if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
        return value[i][j];
    else
        throw rangeError("bad value in getCell");
}

bool board::isSolved()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            if (isBlank(i, j))
            {
                return false;
            }
        }
    }

    return true;
}

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

void board::printConflicts()
{
    for (int i = 1; i <= BoardSize; i++)
    {
        for (int j = 1; j <= BoardSize; j++)
        {
            cout << "Cell (" << i << ", " << j << "): ";
            for (int k = 1; k <= MaxValue; k++)
                cout << conflicts[i][j][k] << " ";
            cout << endl;
        }
    }
}

pair<int, int> board::findEmptyCell()
{
    for (int row = 1; row <= BoardSize; row++)
    {
        for (int col = 1; col <= BoardSize; col++)
        {
            if (isBlank(row, col))
            {
                return make_pair(row, col);
            }
        }
    }

    return make_pair(-1, -1);
}

bool board::solveBoard()
{
    pair<int, int> cell = findEmptyCell();

    if (cell.first == -1 && cell.second == -1)
    {
        return true;
    }

    int i = cell.first;
    int j = cell.second;

    for (int num = MinValue; num <= MaxValue; num++)
    {
        if (conflicts[i][j][num] == 0)
        {
            setCell(i, j, num);

            if (solveBoard())
            {
                return true;
            }

            clearCell(i, j);
        }
    }

    return false;
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
            b1.solveBoard();
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