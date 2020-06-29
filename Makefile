CXX := g++
CXXFLAGS := -Wall -pedantic -std=c++17 -g
LD := g++
LDFLAGS := -Wall -pedantic -std=c++17 -g
SRC := src
OBJ := objects
OBJS := $(OBJ)/main.o $(OBJ)/DirectoryGenerator.o $(OBJ)/RegularGenerator.o $(OBJ)/ScriptGenerator.o $(OBJ)/Request.o $(OBJ)/Response.o $(OBJ)/ConsoleLogger.o $(OBJ)/FileLogger.o $(OBJ)/Logger.o $(OBJ)/SyslogLogger.o $(OBJ)/Cache.o $(OBJ)/Config.o $(OBJ)/Path.o $(OBJ)/Server.o
EXEC := eirserver

.PHONY: all
all: make_objects_dir $(OBJS)

.PHONY: compile
compile: make_objects_dir $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(EXEC)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/generators/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/http/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/loggers/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/server/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: make_objects_dir
make_objects_dir:
	mkdir -p $(OBJ)

.PHONY: run
run:
	./$(EXEC) -c "eirserver.conf"

.PHONY: run_valgrind
run_valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./eirserver -c "eirserver.conf"

.PHONY: clean
clean:
	rm -rf $(EXEC) $(OBJ) doc

$(OBJ)/main.o: $(SRC)/main.cpp $(SRC)/server/Server.h $(SRC)/http/Request.h \
	$(SRC)/server/Config.h $(SRC)/server/Cache.h $(SRC)/loggers/Logger.h \
	$(SRC)/generators/Generator.h $(SRC)/server/Path.h $(SRC)/http/Response.h $(SRC)/http/HttpConstants.h \
	$(SRC)/loggers/Logger.h $(SRC)/server/Config.h $(SRC)/server/Cache.h

$(OBJ)/DirectoryGenerator.o: $(SRC)/generators/DirectoryGenerator.cpp $(SRC)/generators/DirectoryGenerator.h \
	$(SRC)/generators/Generator.h

$(OBJ)/RegularGenerator.o: $(SRC)/generators/RegularGenerator.cpp $(SRC)/generators/RegularGenerator.h \
	$(SRC)/generators/Generator.h

$(OBJ)/ScriptGenerator.o: $(SRC)/generators/ScriptGenerator.cpp $(SRC)/generators/ScriptGenerator.h \
	$(SRC)/generators/Generator.h

$(OBJ)/Request.o: $(SRC)/http/Request.cpp $(SRC)/http/Request.h $(SRC)/server/Config.h \
	$(SRC)/server/Cache.h $(SRC)/loggers/Logger.h $(SRC)/generators/Generator.h $(SRC)/server/Path.h \
	$(SRC)/http/Response.h $(SRC)/http/HttpConstants.h $(SRC)/server/Server.h $(SRC)/http/Request.h \
	$(SRC)/loggers/Logger.h $(SRC)/server/Config.h $(SRC)/server/Cache.h $(SRC)/generators/RegularGenerator.h \
	$(SRC)/generators/Generator.h $(SRC)/generators/DirectoryGenerator.h $(SRC)/generators/ScriptGenerator.h

$(OBJ)/Response.o: $(SRC)/http/Response.cpp $(SRC)/http/Response.h $(SRC)/http/HttpConstants.h

$(OBJ)/ConsoleLogger.o: $(SRC)/loggers/ConsoleLogger.cpp $(SRC)/loggers/ConsoleLogger.h \
	$(SRC)/loggers/Logger.h

$(OBJ)/FileLogger.o: $(SRC)/loggers/FileLogger.cpp $(SRC)/loggers/FileLogger.h \
	$(SRC)/loggers/Logger.h

$(OBJ)/Logger.o: $(SRC)/loggers/Logger.cpp $(SRC)/loggers/Logger.h

$(OBJ)/SyslogLogger.o: $(SRC)/loggers/SyslogLogger.cpp $(SRC)/loggers/SyslogLogger.h \
	$(SRC)/loggers/Logger.h

$(OBJ)/Cache.o: $(SRC)/server/Cache.cpp $(SRC)/server/Cache.h

$(OBJ)/Config.o: $(SRC)/server/Config.cpp $(SRC)/server/Config.h $(SRC)/server/Path.h

$(OBJ)/Path.o: $(SRC)/server/Path.cpp $(SRC)/server/Path.h

$(OBJ)/Server.o: $(SRC)/server/Server.cpp $(SRC)/server/Server.h $(SRC)/http/Request.h \
	$(SRC)/server/Config.h $(SRC)/server/Cache.h \
	$(SRC)/loggers/Logger.h $(SRC)/generators/Generator.h $(SRC)/server/Path.h $(SRC)/http/Response.h \
	$(SRC)/http/HttpConstants.h $(SRC)/loggers/Logger.h \
	$(SRC)/server/Config.h $(SRC)/server/Cache.h $(SRC)/loggers/ConsoleLogger.h \
	$(SRC)/loggers/Logger.h $(SRC)/loggers/SyslogLogger.h $(SRC)/loggers/FileLogger.h