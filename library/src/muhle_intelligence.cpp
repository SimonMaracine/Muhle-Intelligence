#include <ostream>

#include "muhle_intelligence/muhle_intelligence.hpp"

namespace muhle {
    std::ostream& operator<<(std::ostream& stream, const Move& move) {
        switch (move.type) {
            case MoveType::Place:
                stream << "[Place: " << move.place.node_index << ']';
                break;
            case MoveType::Move:
                stream << "[Move: " << move.move.node_source_index << " -> "
                    << move.move.node_destination_index << ']';
                break;
            case MoveType::PlaceTake:
                stream << "[PlaceTake: " << move.place_take.node_index << "; "
                    << move.place_take.node_take_index << ']';
                break;
            case MoveType::MoveTake:
                stream << "[MoveTake: " << move.move_take.node_source_index << " -> "
                    << move.move_take.node_destination_index << "; " << move.move_take.node_take_index << ']';
                break;
        }

        return stream;
    }
}
