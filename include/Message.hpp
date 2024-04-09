#pragma once

#include "dpp/dpp.h"

inline dpp::message make_game_list_message(const dpp::embed& embed)
{
    dpp::message msg(embed);
    msg.add_component(
        dpp::component().add_component(
            dpp::component()
                .set_label("Prev")
                .set_style(dpp::cos_primary)
                .set_id("prev")
        )
        .add_component(
            dpp::component()
                .set_label("Next")
                .set_style(dpp::cos_primary)
                .set_id("next")
        )
    );
    return msg;
}