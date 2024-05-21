using System;
using System.Collections.Generic;

public class ICanvas : IComponent
{
    public enum UiState
    {
        IDLE,
        HOVERED,
        SELECTED,
        HOVEREDSELECTED,
        DISABLED,
        UNKNOWN
    }

    public ICanvas() : base() { }
    public ICanvas(IntPtr gameObjectRef) : base(gameObjectRef) { }

    public void ToggleEnable()
    {
        InternalCalls.CanvasEnableToggle(containerGOptr);
    }

    public int GetSelectedButton()
    {
        return InternalCalls.GetSelectedButton(containerGOptr);
    }

    public int GetSelection()
    {
        return InternalCalls.GetSelected(containerGOptr);
    }

    public void ToggleChecker(bool value, string name)
    {
        InternalCalls.ToggleChecker(containerGOptr, value, name);
    }

    public void PrintItemUI(bool value, string name)
    {
        InternalCalls.PrintItemUI(containerGOptr, value, name);
    }

    public void MoveSelectionButton(int direction)
    {
        InternalCalls.MoveSelectedButton(containerGOptr, direction);
    }

    public void MoveSelection(int direction)
    {
        InternalCalls.MoveSelection(containerGOptr, direction);
    }

    public void ChangeSectImg(string name, int x, int y, int w, int h)
    {
        InternalCalls.ChangeSectImg(containerGOptr, name, x, y, w, h);
    }

    public int GetSliderValue(string name)
    {
        return InternalCalls.GetSliderValue(containerGOptr, name);
    }

    public int GetSliderMaxValue(string name)
    {
        return InternalCalls.GetSliderMaxValue(containerGOptr, name);
    }

    public void SetSliderValue(int value, string name)
    {
        InternalCalls.SetSliderValue(containerGOptr, value, name);
    }

    public void SetUiItemState(UiState state, string name)
    {
        InternalCalls.SetUiItemState(containerGOptr, (int)state, name);
    }

    public string GetTextString(string name)
    {
        return InternalCalls.GetTextString(containerGOptr, name);
    }

    public void SetTextString(string text, string name, int num = -1)
    {
        //List<int> toAdd = new List<int>();
        //List<int> whereToAdd = new List<int>();

        ////first, lets split numbers above 10 since char conversion are from 0 to 9
        ////limit of 99
        //for (int i = 0; i < args.Count; i++)
        //{
        //    if (args[i] >= 10)
        //    {
        //        toAdd.Add(args[i] - (10 * (int)(args[i] / 10)));
        //        args[i] /= 10;
        //        whereToAdd.Add(i + 1 + whereToAdd.Count);
        //    }
        //}
        //for (int i = 0; i < toAdd.Count; i++)
        //{
        //    args.Insert(whereToAdd[i], toAdd[i]);
        //}
        //whereToAdd.Clear();
        //toAdd.Clear();

        ////now that we have it splitted, lets place the splitted numbers into the text string
        //if (args != null)
        //{
        //    for (int i = 0; i < args.Count; i++)
        //    {
        //        text = ReplaceFirst(text, "%d", ("" + (char)(args[i] + 48)));
        //    }
        //}
        InternalCalls.SetTextString(containerGOptr, text, name, num);
    }

    //public static string ReplaceFirst(string text, string search, string replace)
    //{
    //    int pos = text.IndexOf(search);
    //    if (pos < 0)
    //    {
    //        return text;
    //    }
    //    return text.Substring(0, pos) + replace + text.Substring(pos + search.Length);
    //}
}