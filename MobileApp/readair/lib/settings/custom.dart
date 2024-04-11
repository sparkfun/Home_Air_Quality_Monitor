import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/main.dart';
import 'package:readair/settings/display.dart';
import 'package:shared_preferences/shared_preferences.dart';

class CustomizePage extends StatefulWidget {
  @override
  State<CustomizePage> createState() => _CustomizePageState();
}

class DebugController extends GetxController {
  RxBool isDebug = false.obs;

  void toggleDebugMode() {
    isDebug.value = !isDebug.value;
    saveDebugPreference(); // Save the preference when toggling
  }

  Future<void> loadDebugPreference() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    isDebug.value = prefs.getBool('debugMode') ?? false;
  }

  Future<void> saveDebugPreference() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setBool('debugMode', isDebug.value);
  }
}

class _CustomizePageState extends State<CustomizePage> {
  @override
  Widget build(BuildContext context) {
    final BluetoothController bluetoothController = Get.find();
    final ThemeController themeController = Get.find();
    final DebugController debugController = Get.find<DebugController>();

    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Customize"),
      ),
      body: Column(
        children: [
          Obx(() => SwitchListTile(
                title: Text("Dark Mode"),
                value: themeController.isDarkMode.value,
                onChanged: (bool value) {
                  themeController.toggleTheme();
                },
              )),
          Obx(() => SwitchListTile(
                title: Text("Debug Mode"),
                value: debugController.isDebug.value,
                onChanged: (bool value) {
                  debugController.toggleDebugMode();
                },
              )),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 16.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.isSubscribed.value) {
                  Get.to(() => DisplayPage());
                } else {
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(
                      content: Text(
                          "Please connect and subscribe to a device first."),
                    ),
                  );
                }
              },
              child: Text('Go to Display Page'),
            ),
          ),
        ],
      ),
    );
  }
}
