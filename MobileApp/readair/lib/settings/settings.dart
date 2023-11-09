import 'package:flutter/material.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/settings/custom.dart';
import 'package:readair/settings/export.dart'; // Ensure this import is correct for your project setup

class SettingsPage extends StatefulWidget {
  // const SettingsPage({super.key, required this.title});

  // final String title;

  @override
  State<SettingsPage> createState() => _SettingsPageState();
}

class _SettingsPageState extends State<SettingsPage> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Settings"),
      ),
      body: SingleChildScrollView(
        child: Padding(
          padding: const EdgeInsets.all(15.0),
          child: Column(
            children: [
              _settingTile(context, Icons.color_lens, "Customize",
                  "Customize Home Screen, Light/Dark Mode"),
              SizedBox(height: 20),
              _settingTile(context, Icons.bluetooth, "Connect",
                  "WiFi and Bluetooth Connectivity"),
              SizedBox(height: 20),
              _settingTile(context, Icons.share, "Export", "Export data"),
            ],
          ),
        ),
      ),
    );
  }

  Widget _settingTile(
      BuildContext context, IconData icon, String title, String subtitle) {
    return ListTile(
      leading: Icon(icon, size: 40),
      title: Text(title,
          style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
      subtitle: Text(subtitle),
      onTap: () {
        // Define your navigation or actions here based on the clicked setting
        if (title == "Connect") {
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => BluetoothPage()),
          );
        }
        if (title == "Customize") {
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => CustomizePage()),
          );
        }
        if (title == "Export") {
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => ExportPage()),
          );
        }
      },
    );
  }
}
