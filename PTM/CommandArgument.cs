using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public enum CommandArgumentType
    {
        Invalid,
        NumberLiteral,
        StringLiteral,
        VariableIdentifier,
    }

    public class CommandArgument
    {
        public CommandArgumentType Type { set; get; }
        public string Value { set; get; }

        public CommandArgument(CommandArgumentType type, string value)
        {
            Type = type;
            Value = value;
        }
    }
}
