package com.example.varun.csir_trainee;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    EditText url_f;
    EditText url_l;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        url_f = (EditText) findViewById(R.id.url_first);
        url_l = (EditText) findViewById(R.id.url_last);

    }

    public void get_live_data(View view){
        String urlf = url_f.getText().toString().trim();
        String urll = url_l.getText().toString().trim();
        if((urlf.length()!=0)&&(urll.length()!=0)) {
            String urladd = "192.168." + urlf + "." + urll;
            Intent i = new Intent(this, live_data.class);
            i.putExtra("url", urladd);
            startActivity(i);
        }
        else{
            Toast.makeText(this, "Please Enter Proper URL.", Toast.LENGTH_SHORT).show();
        }
    }

    public void get_saved_data(View view){
        Intent i = new Intent(this, saved_data.class);
        startActivity(i);
    }

    public void credit(View view){
        Intent i = new Intent(this, credits.class);
        startActivity(i);
    }

    public void exit(View view){
        finish();
    }
}
