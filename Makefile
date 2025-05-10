# コンパイラとフラグ
CXX = g++
CFLAGS = -Wall -Wextra -std=c++17 -O2 -MMD -MP $(OMP_OPTIONS) $(INCLUDE)
LDFLAGS = $(EXTERNAL_LDFLAGS) $(OMP_LDFLAGS)
LIBS = $(PNG_LIBS) $(OMP_LIBS) $(BOOST_MP_LIBS)

# ディレクトリ
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = obj
BUILD_DIR = build
INCLUDE_DIR = include

# インクルード
EXTERNAL_INCLUDE = -I/opt/homebrew/include
MY_INCLUDE = -I$(INCLUDE_DIR)
OMP_INCLUDE = -I/opt/homebrew/Cellar/libomp/20.1.2/include
INCLUDE = $(EXTERNAL_INCLUDE) $(MY_INCLUDE) $(OMP_INCLUDE)

# リンカ用ディレクトリ
EXTERNAL_LDFLAGS = -L/opt/homebrew/lib
OMP_LDFLAGS = -L/opt/homebrew/Cellar/libomp/20.1.2/lib

# ライブラリ
PNG_LIBS = -lpng16
OMP_LIBS = -lomp
BOOST_MP_LIBS = -lboost_system -lboost_filesystem -lmpc -lmpfr -lgmp

# オプション
OMP_OPTIONS = -Xpreprocessor -fopenmp

# 実行ファイル名
EXE_FILE_NAME = prg

# ソースファイルとオブジェクトファイル
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TESTS = $(wildcard $(TEST_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)  # 依存関係ファイル（.d）

# 実行ファイル
TARGET = $(BUILD_DIR)/$(EXE_FILE_NAME)

# ビルドルール
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

# オブジェクトファイルのルール（依存関係ファイルを同時に生成）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

# obj と build ディレクトリを作成
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 実行
run:
	$(TARGET)

# クリーンアップ
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

# テスト用コンパイル
test: $(TEST_DIR)/test_$(NAME).cpp | $(BUILD_DIR)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR)/test_$(NAME) $(TEST_DIR)/test_$(NAME).cpp $(OBJS)

# 自動生成された依存関係ファイルをインクルード
-include $(DEPS)

.PHONY: run clean test
