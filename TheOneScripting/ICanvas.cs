﻿using System;

public class ICanvas : IComponent
{
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
}