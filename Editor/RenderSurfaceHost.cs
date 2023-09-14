using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Interop;

namespace Editor
{
    public class RenderSurfaceHost : HwndHost
    {
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            IntPtr hwnd = CreateWindowEx(0, "static", GetNum().ToString(), 0x10000000 | 0x40000000, 0, 0, 100, 100, hwndParent.Handle, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
            HandleRef handleRef = new HandleRef(this, hwnd);
            return handleRef;
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DestroyWindow(hwnd.Handle);
        }

        //PInvoke declarations
        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
        internal static extern IntPtr CreateWindowEx(int dwExStyle,
            string lpClassName,
                                                   string lpWindowName,
                                                   int dwStyle,
                                                   int x, int y,
                                                   int nWidth, int nHeight,
                                                   IntPtr hWndParent,
                                                   IntPtr hMenu,
                                                   IntPtr hInstance,
                                                   IntPtr lpParam);

        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Unicode)]
        internal static extern bool DestroyWindow(IntPtr hwnd);

        [DllImport("Vortex.dll", EntryPoint = "GetName", CharSet = CharSet.Unicode)]
        static extern string GetName();

        [DllImport("Vortex.dll", EntryPoint = "GetNum", CharSet = CharSet.Unicode)]
        static extern int GetNum();
    }
}
