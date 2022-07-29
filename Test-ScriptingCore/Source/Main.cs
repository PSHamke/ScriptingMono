using System;
using System.Runtime.CompilerServices;

namespace ScriptingMono
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;    
        }
    }
    public static class InternalCalls
    {
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void CppFunc();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void NativeLog(string message, int value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void NativeLog_Vector(ref Vector3 vec);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static ref Vector3 Vector3_Scale(ref Vector3 vec);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Vector3_Transformation(ref Vector3 vec1, ref Vector3 vec2, out Vector3 result);
    }
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main Constructor!");
            
            Log("Hamke sends param from C#", 123456);

            Vector3 pos = new Vector3(3.4f, 5.1f, 7.7f);
            Log(ref pos);
            // Call first transformation func to multiply by 2 
            Vector3 newVec = Vector3_ScaleGet(ref pos);
            // Log out new vec
            Log(ref newVec);
            // Get Summation of two vector then log result via c++
            Vector3 result = Vector3_TransformationGet(ref newVec, ref newVec);
            Log(ref result);
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

        private void Log(string Text, int param)
        {
            InternalCalls.NativeLog(Text, param);
        }
        private void Log(ref Vector3 vec)
        {
            InternalCalls.NativeLog_Vector( ref vec);
        }

        private Vector3 Vector3_TransformationGet(ref Vector3 vec1, ref Vector3 vec2)
        {
            InternalCalls.Vector3_Transformation(ref vec1, ref vec2, out Vector3 result);
            return result;
        }

        private Vector3 Vector3_ScaleGet(ref Vector3 vec1)
        {
            return InternalCalls.Vector3_Scale(ref vec1); ;
        }

    }
}

