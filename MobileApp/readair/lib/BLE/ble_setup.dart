import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:get/get.dart';
import 'package:readair/BLE/device_details.dart';

class BluetoothPage extends StatefulWidget {
  // const BluetoothPage({super.key, required this.title});

  // final String title;

  @override
  State<BluetoothPage> createState() => _BluetoothPageState();
}

class _BluetoothPageState extends State<BluetoothPage> {
  BluetoothAdapterState blueState = BluetoothAdapterState.unknown;
  late StreamSubscription<BluetoothAdapterState> blue;

  @override
  void initState() {
    super.initState();
    blue = FlutterBluePlus.adapterState.listen((state) {
      blueState = state;
      setState(() {});
    });
    // Check if a device is already connected
    final BluetoothController bluetoothController = Get.find<BluetoothController>();
    bluetoothController.checkConnectedDevice(context);
  }

  @override
  void dispose() {
    blue.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          leading: IconButton(
            icon: Icon(Icons.arrow_back),
            onPressed: () => Navigator.of(context).pop(),
          ),
          title: const Text("Bluetooth"),
        ),
        body: blueState == BluetoothAdapterState.on
            ? const ScanningPage()
            : const BluetoothOffPage());
  }
}

class BluetoothOffPage extends StatelessWidget {
  const BluetoothOffPage({super.key});

  @override
  Widget build(BuildContext context) {
    return const Scaffold(
      body: Center(
        child: Text("Bluetooth is off..."),
      ),
    );
  }
}

class ScanningPage extends StatelessWidget {
  const ScanningPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: GetBuilder<BluetoothController>(
          init: BluetoothController(),
          builder: (controller) {
            return Column(
              children: [
                Center(
                  child: ElevatedButton(
                    onPressed: controller.scanDevices,
                    child: const Text("Scan"),
                  ),
                ),
                const SizedBox(height: 20),
                Expanded(
                  // Wrap StreamBuilder in an Expanded widget
                  child: StreamBuilder(
                    stream: controller.scanRes,
                    builder:
                        (context, AsyncSnapshot<List<ScanResult>> snapshot) {
                      if (snapshot.hasData) {
                        var devicesWithName = snapshot.data!
                            .where((result) =>
                                result.device.name != null &&
                                result.device.name!.isNotEmpty &&
                                result.device.name == "MyESP32")
                            .toList();

                        if (devicesWithName.isEmpty) {
                          return const Center(
                              child: Text("No named devices found!"));
                        }

                        return ListView.builder(
                          itemCount: devicesWithName.length,
                          itemBuilder: (context, i) {
                            return Card(
                              elevation: 2,
                              child: ListTile(
                                title: Text(devicesWithName[i].device.name ??
                                    'Unknown Device'),
                                subtitle: Text(
                                    devicesWithName[i].device.id.toString()),
                                trailing:
                                    Text(devicesWithName[i].rssi.toString()),
                                onTap: () => controller.connectToDevice(
                                    devicesWithName[i].device, context),
                              ),
                            );
                          },
                        );
                      } else {
                        return const Center(child: Text("No devices found!"));
                      }
                    },
                  ),
                ),
              ],
            );
          },
        ),
      ),
    );
  }
}

class BluetoothController extends GetxController {
  BluetoothDevice? connectedDevice;
  List<BluetoothService>? bluetoothServices;

  Future<void> connectToDevice(
      BluetoothDevice device, BuildContext context) async {
    try {
      await device.connect();
      connectedDevice = device;
      discoverServices();

      // Navigate to the Device Details Page
      Get.to(() => DeviceDetailsPage(
            device: device,
            onDisconnect: () => disconnectFromDevice(context),
          ));
    } catch (e) {
      // Handle connection error
      print('Error connecting to device: $e');
    }
  }

  void disconnectFromDevice(BuildContext context) async {
    if (connectedDevice != null) {
      await connectedDevice!.disconnect();
      Get.back(); // Navigate back to the previous screen
    }
  }

  Future<void> discoverServices() async {
    if (connectedDevice != null) {
      bluetoothServices = await connectedDevice!.discoverServices();
      update(); // This triggers a UI update if using GetX
      // Do something with the discovered services, like updating the UI
    }
  }

  void checkConnected(BuildContext context) {
    if (connectedDevice != null) {
           // Navigate to the Device Details Page
           Get.to(() => DeviceDetailsPage(
                 device: connectedDevice!,
                 onDisconnect: () => disconnectFromDevice(context),
             ));
       }
   }

   
  Future<void> checkConnectedDevice(BuildContext context) async {
  var connectedDevices = await FlutterBluePlus.connectedDevices;
  var filteredDevices = connectedDevices.where((d) => d.name == 'MyESP32');

  if (filteredDevices.isNotEmpty) {
    BluetoothDevice targetDevice = filteredDevices.first;
    connectedDevice = targetDevice;
    await discoverServices();
    Get.off(() => DeviceDetailsPage(
      device: targetDevice, 
      onDisconnect: () => disconnectFromDevice(context),
    ));
  }
}


  Future scanDevices() async {
    FlutterBluePlus.startScan(timeout: const Duration(seconds: 5));
    // FlutterBluePlus.stopScan();
  }

  Stream<List<ScanResult>> get scanRes => FlutterBluePlus.scanResults;
}
