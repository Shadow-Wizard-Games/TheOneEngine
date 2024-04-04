﻿using System;
using System.Runtime.CompilerServices;

public class MonoBehaviour : IComponent
{
    public IGameObject attachedGameObject;
    public MonoBehaviour() : base() { attachedGameObject = new IGameObject(containerGOptr); }

    public virtual void Start() { }
    public virtual void Update() { }
}
