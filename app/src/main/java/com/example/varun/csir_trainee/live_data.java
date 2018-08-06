package com.example.varun.csir_trainee;

import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.webkit.DownloadListener;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;
import java.text.DateFormat;
import java.util.Date;

public class live_data extends AppCompatActivity {

    private WebView mWebView;
    String currentDateTimeString = "";
    ProgressBar p;
    TextView connection;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_data);
        Intent intent = getIntent();
        String url;
        url=intent.getStringExtra("url");
        url="http://"+url;

        Thread t = new Thread() {

            @Override
            public void run() {
                try {
                    while (!isInterrupted()) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                updateTime();
                            }
                        });   // updating time every 5 sec
                        Thread.sleep(5000);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        };

        t.start();
        p = (ProgressBar) findViewById(R.id.progressBar);
        connection = (TextView)findViewById(R.id.connection);
        p.setVisibility(View.VISIBLE);
        connection.setVisibility(View.VISIBLE);

        mWebView = (WebView) findViewById(R.id.webView);
        mWebView.setWebViewClient(new MyBrowser());
        mWebView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        connection.setText("Connecting to: "+url);
        mWebView.loadUrl(url);

        ImageButton refresh = (ImageButton) findViewById(R.id.refresh);
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mWebView.reload();
            }
        });
    }

    private class MyBrowser extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            view.loadUrl(url);
            return true;
        }
        @Override
        public void onPageFinished(WebView view, String url) {
            p.setVisibility(View.GONE);
            connection.setVisibility(View.GONE);
        }
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK) && mWebView.canGoBack()) {
            mWebView.goBack(); // Go to previous page
            return true;
        }
        // Use this as else part
        return super.onKeyDown(keyCode, event);
    }


    private void updateTime(){
        TextView datetime = (TextView) findViewById(R.id.datetime);

        currentDateTimeString = DateFormat.getDateTimeInstance().format(new Date());
        datetime.setText("Log Time: " + currentDateTimeString);
    }

    public void backbutton(View view) {
        finish();
    }

    public void log_data(View view) {
        Button back = (Button) findViewById(R.id.back_button_live);
        Button logger = (Button) findViewById(R.id.log_data);
        logger.setVisibility(View.INVISIBLE);
        back.setVisibility(View.INVISIBLE);
        Bitmap b = null;
        LinearLayout layout = (LinearLayout) findViewById(R.id.layout_up);
        b = ScreenshotUtils.getScreenShot(layout);

        if (b != null) {
            File saveFile = ScreenshotUtils.getMainDirectoryName(this);
            File file = ScreenshotUtils.store(b, currentDateTimeString+".png", saveFile);
            Toast.makeText(this, R.string.save_done, Toast.LENGTH_SHORT).show();
        } else
            //If bitmap is null show toast message
            Toast.makeText(this, R.string.save_fail, Toast.LENGTH_SHORT).show();
        logger.setVisibility(View.VISIBLE);
        back.setVisibility(View.VISIBLE);
    }
}