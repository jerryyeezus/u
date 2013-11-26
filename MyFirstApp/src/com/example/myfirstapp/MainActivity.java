package com.example.myfirstapp;

import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.Flushable;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.concurrent.ExecutionException;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends Activity {

    public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";
    private Socket clientSock = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	setContentView(R.layout.activity_main);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
	// Inflate the menu; this adds items to the action bar if it is present.
	getMenuInflater().inflate(R.menu.main, menu);
	return true;
    }

    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {

	// Create intent for sending message to DisplayMessageActivity
	Intent intent = new Intent(this, DisplayMessageActivity.class);

	// Get message from text box
	EditText editText = (EditText) findViewById(R.id.edit_message);
	String str = editText.getText().toString();

	// Send message to server and receive transformed message
	String message = null;
	try {
	    message = new TCPSendMessage().execute(str).get();
	} catch (InterruptedException e) {
	    e.printStackTrace();
	} catch (ExecutionException e) {
	    e.printStackTrace();
	}

	// Put message into intent and send to DisplayMessageActivity
	intent.putExtra(EXTRA_MESSAGE, message);
	startActivity(intent);
    }

    private class TCPSendMessage extends AsyncTask<String, Void, String> {

	protected String doInBackground(String... arg0) {
	    try {
		// Create a new socket
		clientSock = new Socket("130.207.114.21", 12003);

		// Write the message as an output stream
		PrintWriter out = new PrintWriter(new BufferedWriter(
			new OutputStreamWriter(clientSock.getOutputStream())),
			true);
		Message commandMessage = new Message(arg0[0]);
		String encodedMessage = new String();
		encodedMessage = Message.encodeMessage(commandMessage);
		out.println(encodedMessage);

		// Receive the message from the server in unsigned bytes
		DataInputStream input = new DataInputStream(
			clientSock.getInputStream());
		StringBuilder str = new StringBuilder();
		for (int i = 0; i < 2048; i++) {
		    str.append((char) input.readUnsignedByte());
		}
		Message rcvMessage = Message.decodeMessage(str.toString());
		String[] diffNames = DiffClient.fileCompare(rcvMessage);

		Message sndMessage = new Message("pull", diffNames,
			rcvMessage.getCksums());
		encodedMessage = Message.encodeMessage(sndMessage);
		out.println(encodedMessage);
		String musicDir = Environment
			.getExternalStoragePublicDirectory(
				Environment.DIRECTORY_MUSIC).toString();

		// Begin huge PULL loop
		for (int i = 0; i < diffNames.length; i++) {
		    // Get file size
		    String fileStr = new String();
		    char readBuf;
		    for (int j = 0; j < 2048; j++) {
			readBuf = (char) input.readUnsignedByte();
			if (readBuf != 0)
			    fileStr += readBuf;
			else {
			    input.skip(2047 - j);
			    break;
			}
		    }

		    int fSize = Integer.parseInt(fileStr);

		    // Acknowledge file size
		    out.println(encodedMessage);

		    String fpath = musicDir + "/"
			    + rcvMessage.getFileNamesArray()[i];

		    File file = new File(fpath);
		    file.createNewFile();

		    /*
		     * Writer writer = new BufferedWriter(new
		     * OutputStreamWriter( new FileOutputStream(fpath)));
		     */
		    DataOutputStream os = new DataOutputStream(
			    new FileOutputStream(fpath));

		    // char[] buffer = new char[2048];
		    byte[] buffer = new byte[2048];
		    int totBytesRead = 0;
		    int bytesRead = 0;
		    while (totBytesRead < fSize) {
			Arrays.fill(buffer, (byte) 0);
			bytesRead = input.read(buffer);
			totBytesRead += bytesRead;
			os.write(buffer, 0, bytesRead);
		    }
		    os.close();

		    // Send file done ACK
		    out.println(encodedMessage);

		}

		// Print diff string
		String diffStr = new String();
		for (int i = 0; i < diffNames.length; i++) {

		    diffStr += diffNames[i] + "\n";
		}

		return "Differing Files: \n" + diffStr;
		// return "Server Files: \n" + rcvMessage.getFileNames() +
		// "Client Files: \n" + diffNames;
	    } catch (UnknownHostException e) {
		e.printStackTrace();
		Log.d("debugging", "Unknown Host Exception");
	    } catch (IOException e) {
		e.printStackTrace();
		Log.d("debugging", "IO Exception");
	    }
	    return null;
	}

	protected void onPreExecute() {
	}

	protected void onPostExecute(String strOut) {
	}

	protected void onProgressUpdate(Void... values) {
	}

    }

}
