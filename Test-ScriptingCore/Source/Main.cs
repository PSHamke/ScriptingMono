using System;

namespace TestingScript
{
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main Constructor!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }

        public void PrintInt(int param)
        {
            Console.WriteLine("PrintInt from C# : "+ param);
        }

        public void PrintInts(int param1, int param2)
        {
            Console.WriteLine($"C# => Param1: {param1} Param2: {param2}");
        }
        public void PrintCustomMessage(string message)
        {
            Console.WriteLine("Hello world! C# says: " + message);
        }
    }
}

