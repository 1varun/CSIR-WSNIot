package com.example.varun.csir_trainee;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class saved_data extends AppCompatActivity implements AdapterView.OnItemSelectedListener{

    File listFile;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_saved_data);

        Spinner spinner = (Spinner) findViewById(R.id.spinner);
        spinner.setOnItemSelectedListener(this);

        List<String> DateTime = new ArrayList<String>();
        DateTime.add("Choose Date and Time Here.");

        listFile = ScreenshotUtils.getMainDirectoryName(this);
        File[] files = listFile.listFiles();

        for(int i=(files.length-1); i>=0; i--){
            DateTime.add(files[i].getName());
        }

        ArrayAdapter<String> dataAdapter = new ArrayAdapter<String>(this, R.layout.support_simple_spinner_dropdown_item, DateTime);
        dataAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(dataAdapter);
    }

    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id){
        String item = parent.getItemAtPosition(pos).toString();

        ImageView dataView = (ImageView)findViewById(R.id.dataView);
        File imgFile = new File(listFile+"/"+item);
        Bitmap bmp = BitmapFactory.decodeFile(imgFile.getAbsolutePath());

        if(imgFile.exists()){
            dataView.setImageBitmap(bmp);
        }
        else{
            dataView.setImageBitmap(null);
        }

        if(!item.equals("Choose Date and Time Here.")) {
            Toast.makeText(parent.getContext(), "Selected: " + item, Toast.LENGTH_SHORT).show();
        }

    }

    public void onNothingSelected(AdapterView<?> arg0){

    }



    public void backbuttonsaved(View view) {
        finish();
    }
}
