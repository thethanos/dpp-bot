#pragma once

#include "dpp/dpp.h"

inline dpp::message make_random_response_message(const dpp::snowflake& channel_id, const std::string& game_name)
{
    dpp::message msg(channel_id, "Your game is " + game_name);
    msg.add_component(dpp::component()
        .add_component(
            dpp::component()
                .set_label("Sell")
                .set_type(dpp::cot_button)
                //.set_emoji(dpp::unicode_emoji::smile)
                .set_style(dpp::cos_danger)
                .set_id("sell")
        )
        .add_component(
            dpp::component()
                .set_label("Get")
                .set_type(dpp::cot_button)
                //.set_emoji(dpp::unicode_emoji::smile)
                .set_style(dpp::cos_primary)
                .set_id("get")
        )
    );

    return msg;
}

inline dpp::message make_game_list_message(const dpp::embed& embed)
{
    dpp::message msg(embed);
    msg.add_component(dpp::component()
        .add_component(
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