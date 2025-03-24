#include "board.h"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

template<typename T>
static inline void copy_array(T *target, T *source, int size) {
    for (int i = 0; i < size; i++) {
        target[i] = source[i];
    }
}

Board::Board(const Board& that): read_lock{}, write_lock{} {
    this->x_size = that.x_size;
    this->y_size = that.y_size;
    
    int size = that.x_size * that.y_size;
    this->front = new TILE_DISPLAY[size];
    copy_array(this->front, that.front, size);
}

Board& Board::operator=(const Board& that) {
    this->read_lock.lock();

    this->x_size = that.x_size;
    this->y_size = that.y_size;
    
    int size = that.x_size * that.y_size;
    this->front = new TILE_DISPLAY[size];
    copy_array(this->front, that.front, size);

    this->read_lock.unlock();
    
    return *this;
}

Board::Board(Board&& that): read_lock{}, write_lock{} {
    this->x_size = that.x_size;
    this->y_size = that.y_size;
    this->front = that.front;

    that.front = nullptr;
}

Board& Board::operator=(Board&& that) {
    this->read_lock.lock();

    this->x_size = that.x_size;
    this->y_size = that.y_size;
    this->front = that.front;

    that.front = nullptr;

    this->read_lock.unlock();
    
    return *this;
}

Board::~Board() {
    delete this->front;
    delete this->back;
    delete this->boundaries;
}

static inline void place_bomb(TILE_HIDDEN *back, int size, int bomb_count, uint64_t seed) {
    for (int i = 0; i < size; i++) {
        if (i < bomb_count) {
            back[i] = TILE_HIDDEN::BOMB;
            continue;
        }
        back[i] = TILE_HIDDEN::EMPTY;
    }

    std::mt19937 rd{seed};
    std::default_random_engine rng(rd());
    std::shuffle(back, back + size, rng);
}

static inline std::unique_ptr<std::vector<std::pair<int, int>>> find_neighbors(int y, int x, int y_size, int x_size) {
    std::unique_ptr<std::vector<std::pair<int, int>>> output(new std::vector<std::pair<int, int>>());
    int displacement[] = {-1, 0, 1};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 1 & j == 1) continue;
            int row = y + displacement[i];
            int col = x + displacement[j];
            
            if (row == -1 || row == y_size || col == -1 || col == x_size) continue;

            output.get()->push_back(std::make_pair(row, col));
        }
    }
    return output;
}

static inline void calculate_boundary(int *boundaries, TILE_HIDDEN* back, int y_size, int x_size) {
    for (int i = 0; i < y_size; i++) {
        for (int j = 0; j < x_size; j++) {
            if (back[i * x_size + j] == TILE_HIDDEN::BOMB) {
                std::unique_ptr<std::vector<std::pair<int, int>>> neighbors = find_neighbors(i, j, y_size, x_size);
                for (auto i = neighbors.get()->begin(); i != neighbors.get()->end(); i++) boundaries[i->first * x_size + i->second]++;
            }
        }
    }
}

Board::Board(int y_size, int x_size, int bomb_count, uint64_t seed):
y_size{y_size}, x_size{x_size}, front{nullptr}, back{nullptr}, boundaries{nullptr}, read_lock{}, write_lock{}, debug{debug} {
    std::shared_lock<std::shared_mutex> read_guard(read_lock);
    std::unique_lock<std::mutex> write_guard(write_lock);

    if (y_size < 1) throw std::domain_error("y_size must be positive.");
    if (x_size < 1) throw std::domain_error("x_size must be positive.");
    if (bomb_count < 0) throw std::domain_error("bomb_count must be non negative.");
    
    int size = y_size * x_size;
    if (bomb_count > size) throw std::runtime_error("bomb_count cannot be larger than the front size");
    
    front = new TILE_DISPLAY[size] {TILE_DISPLAY::UNTOUCHED};

    back = new TILE_HIDDEN[size] {TILE_HIDDEN::EMPTY};

    boundaries = new int[size] {0};

    place_bomb(back, size, bomb_count, seed);

    calculate_boundary(boundaries, back, y_size, x_size);
}

std::unique_ptr<char[]> Board::print() {
    std::shared_lock<std::shared_mutex> read_guard(read_lock);

    int x_length = x_size + 1;
    int y_length = y_size;
    int size = y_length * x_length;
    std::unique_ptr<char[]> output(new char[size]);
    for (int i = 0; i < y_size; i++) {
        for (int j = 0; j < x_size; j++) {
            if (front[i * x_size + j] == TILE_DISPLAY::UNTOUCHED) output[i * x_length + j] = 'U';
            if (front[i * x_size + j] == TILE_DISPLAY::FLAGGED) output[i * x_length + j] = 'F';
            if (front[i * x_size + j] == TILE_DISPLAY::DUG) output[i * x_length + j] = 'D';
        }
        output[(i + 1) * x_length - 1] = '\n';
    }
    output[size-1] = '\0';
    return output;
}

static inline bool is_out_of_bound(int y, int x, int y_limit, int x_limit) {
    return (y < 0 || x < 0 || y >= y_limit || x >= x_limit);
}

static void recursive_dig(TILE_DISPLAY *front, int *boundaries, int y, int x, int y_size, int x_size) {
    if (front[y * x_size + x] == TILE_DISPLAY::FLAGGED) return;
    if (front[y * x_size + x] == TILE_DISPLAY::DUG) return;
    
    front[y * x_size + x] = TILE_DISPLAY::DUG;

    if (boundaries[y * x_size + x] != 0) return;

    std::unique_ptr<std::vector<std::pair<int, int>>> neighbors = find_neighbors(y, x, y_size, x_size);
    for (auto i = neighbors.get()->begin(); i != neighbors.get()->end(); i++) {
        recursive_dig(front, boundaries, i->first, i->second, y_size, x_size);
    }
}

bool Board::dig(int y, int x) noexcept {
    std::shared_lock<std::shared_mutex> read_guard(read_lock);

    if (is_out_of_bound(y, x , y_size, x_size)) return true;

    if (front[y * x_size + x] == TILE_DISPLAY::DUG || front[y * x_size + x] == TILE_DISPLAY::FLAGGED) return true;
    
    std::unique_lock<std::mutex> write_guard(write_lock);
    if (back[y * x_size + x] == TILE_HIDDEN::EMPTY) {
        recursive_dig(front, boundaries, y, x, y_size, x_size);
        return true;
    }

    back[y * x_size + x] = TILE_HIDDEN::BOMB;
    recursive_dig(front, boundaries, y, x, y_size, x_size);
    std::unique_ptr<std::vector<std::pair<int, int>>> neighbors = find_neighbors(y, x, y_size, x_size);
    for (auto i = neighbors.get()->begin(); i != neighbors.get()->end(); i++) boundaries[i->first * x_size + i->second]--;
    return false;
}

void Board::flag(int y, int x) noexcept {
    if (is_out_of_bound(y, x , y_size, x_size)) return;
    
    if (front[y * x_size + x] == TILE_DISPLAY::UNTOUCHED) {
        front[y * x_size + x] = TILE_DISPLAY::FLAGGED;
    }
}

void Board::deflag(int y, int x) noexcept {
    if (is_out_of_bound(y, x , y_size, x_size)) return;

    std::shared_lock<std::shared_mutex> read_guard(read_lock);
    if (front[y * x_size + x] == TILE_DISPLAY::FLAGGED) {
        std::unique_lock<std::mutex> write_guard(write_lock);
        front[y * x_size + x] = TILE_DISPLAY::UNTOUCHED;
    }
}

std::unordered_map<std::string, std::unique_ptr<char []>> Board::print_debug() {
    int x_length = x_size + 1;
    int y_length = y_size;
    int size = y_length * x_length;
    std::unordered_map<std::string, std::unique_ptr<char []>> output;
    output["front"] = std::unique_ptr<char []> (new char[size]);
    output["back"] = std::unique_ptr<char []> (new char[size]);
    output["boundaries"] = std::unique_ptr<char []> (new char[size]);
    
    for (int i = 0; i < y_size; i++) {
        for (int j = 0; j < x_size; j++) {
            if (front[i * x_size + j] == TILE_DISPLAY::UNTOUCHED) output["front"][i * x_length + j] = 'U';
            if (front[i * x_size + j] == TILE_DISPLAY::FLAGGED) output["front"][i * x_length + j] = 'F';
            if (front[i * x_size + j] == TILE_DISPLAY::DUG) output["front"][i * x_length + j] = 'D';
            
            if (back[i * x_size + j] == TILE_HIDDEN::EMPTY) output["back"][i * x_length + j] = 'E';
            if (back[i * x_size + j] == TILE_HIDDEN::BOMB) output["back"][i * x_length + j] = 'B';

            output["boundaries"][i * x_length + j] = '0' + boundaries[i * x_size + j];
        }
        output["front"][(i + 1) * x_length - 1] = '\n';
        output["back"][(i + 1) * x_length - 1] = '\n';
        output["boundaries"][(i + 1) * x_length - 1] = '\n';
    }
    output["front"][size-1] = '\0';
    output["back"][size-1] = '\0';
    output["boundaries"][size-1] = '\0';
    
    return output;
}