#include <vector>
#include <cassert>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"
#include "muhle_intelligence/internal/various.hpp"
#include "muhle_intelligence/internal/evaluation.hpp"

namespace muhle {
    static constexpr Eval BLACK_ADVANTAGE {-1};
    static constexpr Eval WHITE_ADVANTAGE {1};

    bool all_pieces_in_mills(const Board& board, Piece piece) {
        for (IterIdx i {0}; i < NODES; i++) {
            if (board[i] != piece) {
                continue;
            }

            if (!is_mill(board, piece, i)) {
                return false;
            }
        }

        return true;
    }

#define IS_FREE(const_index) \
    if (board[const_index] == Piece::None) { \
        result.push_back(const_index); \
    }

    Array<Idx, 4> neighbor_free_positions(const Board& board, Idx index) {
        Array<Idx, 4> result;

        switch (index) {
            case 0:
                IS_FREE(1)
                IS_FREE(9)
                break;
            case 1:
                IS_FREE(0)
                IS_FREE(2)
                IS_FREE(4)
                break;
            case 2:
                IS_FREE(1)
                IS_FREE(14)
                break;
            case 3:
                IS_FREE(4)
                IS_FREE(10)
                break;
            case 4:
                IS_FREE(1)
                IS_FREE(3)
                IS_FREE(5)
                IS_FREE(7)
                break;
            case 5:
                IS_FREE(4)
                IS_FREE(13)
                break;
            case 6:
                IS_FREE(7)
                IS_FREE(11)
                break;
            case 7:
                IS_FREE(4)
                IS_FREE(6)
                IS_FREE(8)
                break;
            case 8:
                IS_FREE(7)
                IS_FREE(12)
                break;
            case 9:
                IS_FREE(0)
                IS_FREE(10)
                IS_FREE(21)
                break;
            case 10:
                IS_FREE(3)
                IS_FREE(9)
                IS_FREE(11)
                IS_FREE(18)
                break;
            case 11:
                IS_FREE(6)
                IS_FREE(10)
                IS_FREE(15)
                break;
            case 12:
                IS_FREE(8)
                IS_FREE(13)
                IS_FREE(17)
                break;
            case 13:
                IS_FREE(5)
                IS_FREE(12)
                IS_FREE(14)
                IS_FREE(20)
                break;
            case 14:
                IS_FREE(2)
                IS_FREE(13)
                IS_FREE(23)
                break;
            case 15:
                IS_FREE(11)
                IS_FREE(16)
                break;
            case 16:
                IS_FREE(15)
                IS_FREE(17)
                IS_FREE(19)
                break;
            case 17:
                IS_FREE(12)
                IS_FREE(16)
                break;
            case 18:
                IS_FREE(10)
                IS_FREE(19)
                break;
            case 19:
                IS_FREE(16)
                IS_FREE(18)
                IS_FREE(20)
                IS_FREE(22)
                break;
            case 20:
                IS_FREE(13)
                IS_FREE(19)
                break;
            case 21:
                IS_FREE(9)
                IS_FREE(22)
                break;
            case 22:
                IS_FREE(19)
                IS_FREE(21)
                IS_FREE(23)
                break;
            case 23:
                IS_FREE(14)
                IS_FREE(22)
                break;
        }

        return result;
    }

#define IS_PC(const_index) (board[const_index] == piece)

    bool is_mill(const Board& board, Piece piece, Idx index) {
        assert(piece != Piece::None);

        switch (index) {
            case 0:
                if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                    return true;
                break;
            case 1:
                if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                    return true;
                break;
            case 2:
                if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                    return true;
                break;
            case 3:
                if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                    return true;
                break;
            case 4:
                if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                    return true;
                break;
            case 5:
                if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                    return true;
                break;
            case 6:
                if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                    return true;
                break;
            case 7:
                if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                    return true;
                break;
            case 8:
                if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                    return true;
                break;
            case 9:
                if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                    return true;
                break;
            case 10:
                if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                    return true;
                break;
            case 11:
                if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                    return true;
                break;
            case 12:
                if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                    return true;
                break;
            case 13:
                if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                    return true;
                break;
            case 14:
                if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                    return true;
                break;
            case 15:
                if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                    return true;
                break;
            case 16:
                if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                    return true;
                break;
            case 17:
                if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                    return true;
                break;
            case 18:
                if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                    return true;
                break;
            case 19:
                if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                    return true;
                break;
            case 20:
                if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                    return true;
                break;
            case 21:
                if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                    return true;
                break;
            case 22:
                if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                    return true;
                break;
            case 23:
                if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                    return true;
                break;
        }

        return false;
    }

    bool is_game_over(const SearchNode& node, Eval& evaluation_game_over) {
        if (node.plies < 18) {
            return false;
        }

        if (node.white_pieces_on_board < 3) {
            evaluation_game_over = BLACK_ADVANTAGE;
            return true;
        }

        if (node.black_pieces_on_board < 3) {
            evaluation_game_over = WHITE_ADVANTAGE;
            return true;
        }

        unsigned int white_free_positions {0};
        unsigned int black_free_positions {0};
        get_players_freedom(node, white_free_positions, black_free_positions);

        if (white_free_positions == 0) {
            evaluation_game_over = BLACK_ADVANTAGE;
            return true;
        }

        if (black_free_positions == 0) {
            evaluation_game_over = WHITE_ADVANTAGE;
            return true;
        }

        return false;
    }

    unsigned int pieces_on_board(const SearchNode& node, Piece piece) {
        assert(piece != Piece::None);

        switch (piece) {
            case Piece::White:
                return node.white_pieces_on_board;
            case Piece::Black:
                return node.black_pieces_on_board;
            default:
                break;
        }

        return 0;
    }

    Player opponent(Player player) {
        switch (player) {
            case Player::White:
                return Player::Black;
            case Player::Black:
                return Player::White;
        }

        return {};
    }

    Piece opponent_piece(Piece piece) {
        assert(piece != Piece::None);

        switch (piece) {
            case Piece::White:
                return Piece::Black;
            case Piece::Black:
                return Piece::White;
            default:
                break;
        }

        return {};
    }

    void count_pieces(const Board& board, unsigned int& white, unsigned int& black) {
        for (IterIdx i {0}; i < NODES; i++) {
            switch (board[i]) {
                case Piece::White:
                    white++;
                    break;
                case Piece::Black:
                    black++;
                    break;
                default:
                    break;
            }
        }
    }
}
