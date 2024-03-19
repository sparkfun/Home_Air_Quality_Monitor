import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:readair/main.dart';
import 'package:readair/settings/display.dart';

class CustomizePage extends StatefulWidget {
  @override
  State<CustomizePage> createState() => _CustomizePageState();
}

class _CustomizePageState extends State<CustomizePage> {
  @override
  Widget build(BuildContext context) {
    final ThemeController themeController = Get.find();

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
                          Padding(
              padding: const EdgeInsets.symmetric(vertical: 16.0),
              child: ElevatedButton(
                onPressed: () {
                  // Navigate to DisplayPage when the button is pressed
                  Get.to(() => DisplayPage());
                },
                child: Text('Go to Display Page'),
              ),
            ),
        ],
      ),
    );
  }
}
