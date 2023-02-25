#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <ncurses.h>
using namespace std; //Boo hiss

class Map {
    vector<vector<char>> map;
    default_random_engine gen;
    public:
    void set_char(int x, int y, char character) {
        map.at(y).at(x) = character;
    }
    char get_char(int x, int y) {
        if (map.at(y).at(x) == HERO) {
            return HERO;
        }
        else if (map.at(y).at(x) == MONSTER) {
            return MONSTER;
        }
        else if (map.at(y).at(x) == WALL) {
            return WALL;
        }
        else if (map.at(y).at(x) == WATER) {
            return WATER;
        }
        else if (map.at(y).at(x) == OPEN) {
            return OPEN;
        }
        else if (map.at(y).at(x) == SAVE) {
            return SAVE;
        }
        else {
            return TREASURE;
        }
    }
  
    fstream saveFile;
    void saveMap() {
        saveFile.open("mapSave.txt", ios::out);
        if (saveFile.is_open()) {
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    saveFile << map.at(i).at(j);
                    if (j == SIZE - 1) saveFile << endl;
                }
            }
        }
        saveFile.close();
    }
    void loadMap() {
        string line;
        saveFile.open("mapSave.txt", ios::in);
        if (saveFile.is_open()) {
            while (getline(saveFile, line)) {
                vector<char> row;
                for (char &c : line) row.push_back(c);
                map.push_back(row);
            }
        }
        saveFile.close();
    }

    static const char HERO     = 'H';
    static const char MONSTER  = 'M';
    static const char WALL     = '#';
    static const char WATER    = '~';
    static const char OPEN     = '.';
    static const char TREASURE = '$';
    static const size_t SIZE = 100; //World is a 100x100 map
    static const size_t DISPLAY = 30; //Show a 30x30 area at a time
    static const char SAVE = 'S';
    
    //Randomly generate map
    void init_map() {
        uniform_int_distribution<int> d100(1,100);
        map.clear();
        map.resize(SIZE); //100 rows tall
        for (auto &v : map) v.resize(SIZE,'.'); //100 columns wide
        for (size_t i = 0; i < SIZE; i++) {
            for (size_t j = 0; j < SIZE; j++) {
                map.at(10).at(10) = SAVE;
                map.at(90).at(90) = SAVE;
                map.at(90).at(10) = SAVE;
                map.at(10).at(90) = SAVE;

                if (i == 0 or j == 0 or i == SIZE-1 or j == SIZE-1)
                    map.at(i).at(j) = WALL;
                else if (i == SIZE/2 and j == SIZE/2)
                    map.at(i).at(j) = HERO;
                else {
                    //5% chance of monster
                    if (d100(gen) <= 5) {
                        map.at(i).at(j) = MONSTER;
                    }
                    else if (d100(gen) <= 3) {
                        map.at(i).at(j) = TREASURE;
                    }
                    else if (d100(gen) <= 10) { //10% each spot is wall
                        map.at(i).at(j) = WALL;
                    }
                    else if (d100(gen) <= 3) { //3% each spot is water
                        map.at(i).at(j) = WATER;
                    }
                    else if (d100(gen) <= 40) { //40% chance of water near other water
                        if (map.at(i-1).at(j) == WATER or map.at(i+1).at(j) == WATER or map.at(i).at(j-1) == WATER or map.at(i).at(j+1) == WATER)
                            map.at(i).at(j) = WATER;
                    }
                }
            }
        }
    }
  
    //Draw the DISPLAY tiles around coordinate (x,y)
    void draw(int x, int y) {
        int start_x = x - DISPLAY/2;
        int end_x = x + DISPLAY/2;
        int start_y = y - DISPLAY/2;
        int end_y = y + DISPLAY/2;

        //Bounds check to handle the edges
        if (start_x < 0) {
            end_x = end_x - start_x;
            start_x = 0;
        }
        if (end_x > SIZE-1) {
            start_x = start_x - (end_x - (SIZE-1));
            end_x = SIZE-1;
        }
        if (start_y < 0) {
            end_y = end_y - start_y;
            start_y = 0;
        }
        if (end_y > SIZE-1) {
            start_y = start_y - (end_y - (SIZE-1));
            end_y = SIZE-1;
        }

        //Now draw the map using NCURSES
        for (size_t i = start_y; i <= end_y; i++) {
            for (size_t j = start_x; j <= end_x; j++) {
                if (i == y && j == x) {
                    attron(A_UNDERLINE | A_BOLD);
                    mvaddch(i-start_y,j-start_x,'H');
                    attroff(A_UNDERLINE | A_BOLD);
                }
                else {
                    int color = 1;
                    if (map.at(i).at(j) == WALL)
                        color = 5;
                    else if (map.at(i).at(j) == WATER)
                        color = 2;
                    else if (map.at(i).at(j) == HERO)
                        color = 3;
                    else if (map.at(i).at(j) == TREASURE)
                        color = 4;
                    else if (map.at(i).at(j) == MONSTER)
                        color = 6;
                    else if (map.at(i).at(j) == SAVE)
                        color = 7;

                    attron(COLOR_PAIR(color));
                    mvaddch(i-start_y,j-start_x,map.at(i).at(j));
                    attroff(COLOR_PAIR(color));
                }
            }
        }
    }
    Map() {
        //init_map();
    }
};
