#include "EmtpyEconomy.h"


namespace land ::economy::detail {


EmtpyEconomy::EmtpyEconomy() = default;

llong EmtpyEconomy::get(Player&) const { return 0; }
llong EmtpyEconomy::get(mce::UUID const&) const { return 0; }

bool EmtpyEconomy::set(Player&, llong) const { return true; }
bool EmtpyEconomy::set(mce::UUID const&, llong) const { return true; }

bool EmtpyEconomy::add(Player&, llong) const { return true; }
bool EmtpyEconomy::add(mce::UUID const&, llong) const { return true; }

bool EmtpyEconomy::reduce(Player&, llong) const { return true; }
bool EmtpyEconomy::reduce(mce::UUID const&, llong) const { return true; }

bool EmtpyEconomy::transfer(Player&, Player&, llong) const { return true; }
bool EmtpyEconomy::transfer(mce::UUID const&, mce::UUID const&, llong) const { return true; }


} // namespace land::economy::detail