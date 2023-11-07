import 'package:flutter/material.dart';

class CustomizePage extends StatefulWidget {
  // const CustomizePage({super.key, required this.title});

  // final String title;

  @override
  State<CustomizePage> createState() => _CustomizePageState();
}

class _CustomizePageState extends State<CustomizePage> {

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Customize"),
      ),
      body: SingleChildScrollView(
      ),
    );
  }


}
