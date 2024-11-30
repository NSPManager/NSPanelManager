import React from 'react';
import Layout from '@theme/Layout';
import ESPFlashingTool from "@site/src/components/ESPFlashingTool";

const FlashESP32 = () => {
  return (
      <Layout
          title="Flash ESP32"
          description="Flash your ESP32 device using a web-based tool">
        <div style={{ textAlign: 'center', padding: '2rem' }}>
          <h1>Flash Your NSPanel</h1>
          <p>
            Use this tool to flash firmware onto your NSPanel device. Please ensure
            you are using a supported browser (e.g., Chrome or Edge) that
            supports the Web Serial API.
          </p>
          <div>
            <ESPFlashingTool

            />
          </div>


          Installer powered by <a href="https://esphome.github.io/esp-web-tools/">ESP Web Tools</a>.
        </div>
      </Layout>
  );
};

export default FlashESP32;
