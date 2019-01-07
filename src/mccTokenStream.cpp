#include <utility>

//
// Created by cub3d on 07/01/19.
//

#include "mccTokenStream.h"

mccTokenStream::mccTokenStream(std::vector<mccToken> tokens) : mccVectorStream(std::move(tokens)) {}
