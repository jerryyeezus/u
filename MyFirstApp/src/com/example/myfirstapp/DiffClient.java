package com.example.myfirstapp;

import java.io.File;
import java.util.ArrayList;

import android.os.Environment;
import android.util.Log;

public class DiffClient {

    Message rcvMessage;

    public DiffClient(Message rcvMessage) {
	rcvMessage = this.rcvMessage;
    }

    public static void main(String[] args) {

    }

    public static String[] fileCompare(Message rcvMessage) {

	ArrayList<String> diffList = new ArrayList<String>();
	boolean isFound;

	/* Get server file names from received message */
	String[] serverNames = rcvMessage.getFileNameArray();

	/* Get client file names from local directory */
	String musicDir = Environment.getExternalStoragePublicDirectory(
		Environment.DIRECTORY_MUSIC).toString();
	File clientFiles = new File(musicDir);
	String[] clientNames = getLocalFiles(clientFiles);

	for (int i = 0; i < serverNames.length; i++) {
	    isFound = false;
	    //Log.d("debugging", "SL: " + (serverNames.length - 1));
	    for (int j = 0; j < clientNames.length; j++) {
		Log.d("debugging", "Server: " + serverNames[j]);
		if (clientNames[i].equals(serverNames[j])) {
		    isFound = true;
		    break;
		}
	    }
	    if (!isFound) {
		diffList.add(serverNames[i]);
	    }
	}

	String[] diffNames = new String[diffList.size()];
	for (int i = 0; i < diffNames.length; i++) {
	    diffNames[i] = diffList.get(i).toString();
	}

	return diffNames;
    }

    public static String[] getLocalFiles(final File folder) {
	ArrayList<String> fileList = new ArrayList<String>();
	if (folder.listFiles() == null)
	    return new String[0];
	
	for (final File fileEntry : folder.listFiles()) {
	    if (fileEntry.isDirectory()) {
		getLocalFiles(fileEntry);
	    } else {
		fileList.add(fileEntry.getName());
	    }
	}
	String[] fileNames = new String[fileList.size()];
	for (int i = 0; i < fileNames.length; i++) {
	    fileNames[i] = fileList.get(i).toString();
	}
	return fileNames;
    }

}
