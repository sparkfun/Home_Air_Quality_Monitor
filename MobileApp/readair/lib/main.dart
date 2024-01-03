import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:get/get.dart';
import 'package:provider/provider.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/BLE/ble_states.dart';
import 'package:readair/data/packet.dart';
import 'dart:io';

import 'package:readair/homescreen/home.dart';

void main() {
  Get.put(BluetoothController());
  runApp(const MyApp());
}

// void main() {
//   runApp(
//     ChangeNotifierProvider(
//       create: (context) => BluetoothConnectionProvider(),
//       child: const MyApp(), // Your main app widget
//     ),
//   );
// }

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
      title: 'ReadAIR',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.redAccent),
        useMaterial3: true,
      ),
      home: MyHomePage(title: 'ReadAIR App v0.01'),
      //home: MyHomePage(title: 'ReadAIR App v0.01', data: DatabaseService.instance.getLastPacket(),),
      // navigatorObservers: [BluetoothAdapterStateObserver()],
    );
  }
}



class BluetoothAdapterStateObserver extends NavigatorObserver {
  StreamSubscription<BluetoothAdapterState>? blueState;

  @override
  void didPush(Route route, Route? previousRoute) {
    super.didPush(route, previousRoute);
    if (route.settings.name == '/DeviceScreen') {
      // Start listening to Bluetooth state changes when a new route is pushed
      blueState ??= FlutterBluePlus.adapterState.listen((state) {
        if (state != BluetoothAdapterState.on) {
          // Pop the current route if Bluetooth is off
          navigator?.pop();
        }
      });
    }
  }

  @override
  void didPop(Route route, Route? previousRoute) {
    super.didPop(route, previousRoute);
    // Cancel the subscription when the route is popped
    blueState?.cancel();
    blueState = null;
  }
}