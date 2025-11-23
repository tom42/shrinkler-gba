// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

export module shrinkler_gba:gba_packer;
import :console;
import :options;

namespace shrinkler_gba
{

// TODO: not sure this should even be a class
export class gba_packer final
{
public:
    void pack(const options& options);

private:
    void initialize_console_output();
    void load_input_file();

    console m_console;
};

}
