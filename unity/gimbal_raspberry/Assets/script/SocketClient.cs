using System;
using System.Collections;
using System.Net.Sockets;
using UnityEngine;

public class SocketClient : MonoBehaviour
{
    public String server_ip = "cloud.park-cloud.co19.kr";
    public int server_port = 8090;

    private TcpClient client;
    private NetworkStream stream;
    private byte[] buffer = new byte[1024];
    private bool isConnected = false;

    private void Start()
    {
        try
        {
            client = new TcpClient(server_ip, server_port); // 서버 IP 주소와 포트 번호 설정
            stream = client.GetStream();
            isConnected = true;

            // 비동기 수신 시작
            stream.BeginRead(buffer, 0, buffer.Length, ReceiveCallback, null);

            // 일정 시간마다 메시지를 보내는 코루틴 시작
            StartCoroutine(SendMessageCoroutine());
        }
        catch (Exception e)
        {
            Debug.Log("Connection error: " + e.Message);
        }
    }

    private void ReceiveCallback(IAsyncResult result)
    {
        try
        {
            int bytesRead = stream.EndRead(result);
            string message = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
            Debug.Log("Server: " + message);

            // 다시 비동기 수신 시작
            stream.BeginRead(buffer, 0, buffer.Length, ReceiveCallback, null);
        }
        catch (Exception e)
        {
            Debug.Log("Receive error: " + e.Message);
        }
    }

    private IEnumerator SendMessageCoroutine()
    {
        while (isConnected)
        {
            yield return new WaitForSeconds(0.3f); // 메시지를 보낼 간격 설정 1f = 1초 
            Vector3 rotation = transform.rotation.eulerAngles;
            Debug.Log("Camera Rotation: "); //+ rotation);
            SendMessage("Hello from Unity! rotaion : " + rotation);
        }
    }

    private void SendMessage(string message)
    {
        try
        {
            if (!isConnected)
            {
                Debug.Log("Not connected to server.");
                return;
            }

            byte[] data = System.Text.Encoding.UTF8.GetBytes(message);
            stream.Write(data, 0, data.Length);
        }
        catch (Exception e)
        {
            Debug.Log("Send error: " + e.Message);
        }
    }

    private void OnDestroy()
    {
        if (stream != null)
        {
            stream.Close();
        }
        if (client != null)
        {
            client.Close();
        }
    }
}