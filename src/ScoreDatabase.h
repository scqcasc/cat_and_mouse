#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>

struct HighScoreEntry {
    int id;
    std::string initials;
    int score;
    int levelsCompleted;
    std::string dateTime;
};

class ScoreDatabase {
public:
    static ScoreDatabase& Instance();

    bool Initialize(const std::string& dbPath = "highscores.db");
    void Close();

    bool SaveScore(const std::string& initials, int score, int levelsCompleted);
    std::vector<HighScoreEntry> GetTopScores(int limit = 10);

private:
    ScoreDatabase() = default;
    ~ScoreDatabase();

    sqlite3* db = nullptr;
    bool isInitialized = false;

    std::string GetCurrentDateTime();
};
