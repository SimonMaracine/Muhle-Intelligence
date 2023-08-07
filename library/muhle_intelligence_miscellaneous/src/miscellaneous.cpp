#include <string>
#include <cassert>
#include <ostream>
#include <sstream>
#include <iomanip>

#include "muhle_intelligence/miscellaneous.hpp"

namespace muhle {
    static const char* node_index_to_name(int index) {
        switch (index) {
            case 0:
                return "a7";
            case 1:
                return "d7";
            case 2:
                return "g7";
            case 3:
                return "b6";
            case 4:
                return "d6";
            case 5:
                return "f6";
            case 6:
                return "c5";
            case 7:
                return "d5";
            case 8:
                return "e5";
            case 9:
                return "a4";
            case 10:
                return "b4";
            case 11:
                return "c4";
            case 12:
                return "e4";
            case 13:
                return "f4";
            case 14:
                return "g4";
            case 15:
                return "c3";
            case 16:
                return "d3";
            case 17:
                return "e3";
            case 18:
                return "b2";
            case 19:
                return "d2";
            case 20:
                return "f2";
            case 21:
                return "a1";
            case 22:
                return "d1";
            case 23:
                return "g1";
            default:
                assert(false);
                break;
        }

        return nullptr;
    }

    std::string move_to_string(const Move& move, Player player) {
        std::string result;

        switch (player) {
            case Player::White:
                result.push_back('W');
                break;
            case Player::Black:
                result.push_back('B');
                break;
        }

        switch (move.type) {
            case MoveType::Place:
            case MoveType::PlaceTake:
                result.push_back('P');
                break;
            case MoveType::Move:
            case MoveType::MoveTake:
                result.push_back('M');
                break;
        }

        switch (move.type) {
            case MoveType::Place:
                result.append(node_index_to_name(move.place.node_index));

                break;
            case MoveType::Move:
                result.append(node_index_to_name(move.move.node_source_index));
                result.push_back('-');
                result.append(node_index_to_name(move.move.node_destination_index));

                break;
            case MoveType::PlaceTake:
                result.append(node_index_to_name(move.place_take.node_index));
                result.push_back('T');
                result.append(node_index_to_name(move.place_take.node_take_index));

                break;
            case MoveType::MoveTake:
                result.append(node_index_to_name(move.move_take.node_source_index));
                result.push_back('-');
                result.append(node_index_to_name(move.move_take.node_destination_index));
                result.push_back('T');
                result.append(node_index_to_name(move.move_take.node_take_index));

                break;
        }

        return result;
    }

    void print_result_statistics(const Result& result, std::ostream& stream) {
        stream << "Time seconds: " << std::setprecision(3) << result.time << '\n';
        stream << "Evaluation: " << result.evaluation << '\n';
        stream << "Positions evaluated: " << result.positions_evaluated << '\n';
    }

    void print_result_statistics(const Result& result, std::string& string) {
        std::stringstream stream;

        stream << "Time seconds: " << std::setprecision(3) << result.time << '\n';
        stream << "Evaluation: " << result.evaluation << '\n';
        stream << "Positions evaluated: " << result.positions_evaluated << '\n';

        string = stream.str();
    }
}
