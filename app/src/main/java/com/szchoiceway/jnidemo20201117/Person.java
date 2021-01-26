package com.szchoiceway.jnidemo20201117;

import android.util.Log;

/**
 * create by FrankP on 2021/1/26
 */
public class Person {
    private int age;
    private String name;

    public Person(int age, String name) {
        this.age = age;
        this.name = name;
    }

    public void print() {
        Log.e("Person", name + age + "岁了");
    }
}
