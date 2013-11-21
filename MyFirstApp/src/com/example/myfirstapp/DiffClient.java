package com.example.myfirstapp;

import java.io.File;

public class DiffClient {

	Message rcvMessage;
	
	public DiffClient(Message rcvMessage) {
		rcvMessage = this.rcvMessage;
	}

	public static void main(String[] args) {
		
	}
	
	public String fileCompare(Message rcvMessage) {
		
	}
	
	public String listFilesForFolder(final File folder) {
		String fileStr = new String();
        for (final File fileEntry : folder.listFiles()) {
            if (fileEntry.isDirectory()) {
                listFilesForFolder(fileEntry);
            } else {
                fileStr += fileEntry.getName() + "\n";
            }
        }
        return fileStr;
    }

}
