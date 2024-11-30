import React from 'react';
import styles from './styles.module.css';

const ESPFlashingTool = ({  }) => {
    return (
        <div style={{marginTop: '2rem'}}>

          <p>Firmware version</p>
          <select className="radios">
            <optgroup label="Stable"/>
            <option value="1.0">1.0</option>
            <optgroup label="Unstable"/>
            <option value="beta">beta</option>
          </select>

          <esp-web-install-button
              manifest="https://firmware.esphome.io/esp-web-tools/manifest.json"
          >

            <button slot="activate">Flash NSPanel firmware</button>


            <p>
              <p slot="unsupported">Ah snap, your browser doesn't work!</p>
              <p slot="not-allowed">Ah snap, you are not allowed to use this on HTTP!</p>
            </p>


          </esp-web-install-button>

        </div>
    );
};

export default ESPFlashingTool;