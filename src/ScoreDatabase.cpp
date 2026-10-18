#include "ScoreDatabase.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

ScoreDatabase& ScoreDatabase::Instance() {
    static ScoreDatabase instance;
    return instance;
}

ScoreDatabase::~ScoreDatabase() {
    Close();
}

bool ScoreDatabase::Initialize(const std::string& dbPath) {
    if (isInitialized) return true;

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "ScoreDatabase Error: Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Create high_scores table if it doesn't exist
    const char* createTableSQL = 
        "CREATE TABLE IF NOT EXISTS high_scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "initials TEXT NOT NULL, "
        "score INTEGER NOT NULL, "
        "levels INTEGER NOT NULL, "
        "timestamp TEXT NOT NULL);";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "ScoreDatabase Error: Failed to create table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    isInitialized = true;
    std::cout << "ScoreDatabase initialized successfully (" << dbPath << ")." << std::endl;
    return true;
}

void ScoreDatabase::Close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    isInitialized = false;
}

std::string ScoreDatabase::GetCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuffer;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tmBuffer, &nowTime);
#else
    localtime_r(&nowTime, &tmBuffer);
#endif

    std::ostringstream ss;
    ss << std::put_time(&tmBuffer, "%Y-%m-%d %H:%M");
    return ss.str();
}

bool ScoreDatabase::SaveScore(const std::string& initials, int score, int levelsCompleted) {
    if (!isInitialized || !db) return false;

    std::string timestamp = GetCurrentDateTime();
    const char* insertSQL = "INSERT INTO high_scores (initials, score, levels, timestamp) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "ScoreDatabase Error: Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, initials.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, score);
    sqlite3_bind_int(stmt, 3, levelsCompleted);
    sqlite3_bind_text(stmt, 4, timestamp.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "ScoreDatabase Error: Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

std::vector<HighScoreEntry> ScoreDatabase::GetTopScores(int limit) {
    std::vector<HighScoreEntry> scores;
    if (!isInitialized || !db) return scores;

    const char* selectSQL = "SELECT id, initials, score, levels, timestamp FROM high_scores ORDER BY score DESC LIMIT ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "ScoreDatabase Error: Select prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return scores;
    }

    sqlite3_bind_int(stmt, 1, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HighScoreEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.initials = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.score = sqlite3_column_int(stmt, 2);
        entry.levelsCompleted = sqlite3_column_int(stmt, 3);
        entry.dateTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        scores.push_back(entry);
    }

    sqlite3_finalize(stmt);
    return scores;
}
