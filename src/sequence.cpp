#include "utils.hpp"
#include "sequence.hpp"
#include "sequencer.hpp"

Sequence::Sequence(){} // empty default constructor for std::map

Sequence::Sequence(Sequencer *seq, std::string osc_address, const char* osc_type, std::map<int, double> v, int seq_length, bool state, bool is_note)
{

    sequencer = seq;

    address = osc_address;
    type = osc_type;

    enabled = state;
    values = v;
    length = seq_length;

    note = is_note;
    note_on = false;

    feed_status(false);

}

Sequence::~Sequence()
{

    disable();
    feed_status(true);

}

void Sequence::enable()
{

    enabled = true;
    feed_status(false);

}

void Sequence::disable()
{

    note_off();

    enabled = false;
    feed_status(false);

}

void Sequence::toggle()
{

    if (enabled) {
        disable();
    } else {
        enable();
    }

}

void Sequence::play(int c)
{

    if (!enabled || !length) return;

    int cursor = c % length;

    if (values.find(cursor) != values.end()) {

        sequencer->osc_send(address, type, values[cursor]);

        if (note) {
            note_on = values[cursor] > 0;
        }

    }

}

void Sequence::note_off()
{

    if (!enabled || !note_on) return;

    note_on = false;
    sequencer->osc_send(address, type, 0);

}

void Sequence::feed_status(bool deleted)
{

    std::string json = "{";

    json += "\"address\":\"" + address + "\",";

    if (deleted) {

        json += "\"removed\":true";

    } else {

        json += "\"enabled\":" + bool_to_str(enabled) + ",";
        json += "\"type\":\"" + (std::string)type + "\",";
        json += "\"note\":" + bool_to_str(note) + ",";
        json += "\"length\":" + std::to_string(length) + ",";
        // if (note) json += "\"note_on\":" + bool_to_str(note_on) + ",";

        json += "\"values\":{" ;

        for (auto it2 = values.cbegin(); it2 != values.cend();) {
            json += "\"" + std::to_string(it2->first) + "\":" + std::to_string(it2->second);
            it2++;
            if (it2 != values.cend()) json += ",";
        }

        json += "}";

    }

    json += "}";

    sequencer->osc_send_feed("/status/sequence", json);

}
