import { useEffect, useState } from "react";
import { useStompStore } from "../../stores/StompStore";
import { useSettingsStore } from "../../stores/SettingsStore";

interface INSpanelLogData {
  time: string;
  level: "ERROR" | "WARNING" | "INFO" | "DEBUG" | "TRACE";
  mac_address: string;
  message: string;
  panel: string; // Panel name
  type: "log";
}

export default function NSPanelLogs({ nspanel_mac }: { nspanel_mac: string }) {
  const { status: settingsStatus, settings, fetchData: fetchSettingsData } = useSettingsStore();
  const { status: stompStatus } = useStompStore();
  const [logs, setLogs] = useState([]);
  const [hasSubscribed, setHasSubscribed] = useState(false);
  const [hasDownloadedLogs, setHasDownloadedLogs] = useState(false);
  const maxLogs = useSettingsStore.getState().settings?.max_live_log_messages ?? 250;

  const nspanel_backlog_callback = (message: any) => {
    useStompStore.getState().unsubscribe(`nspanel/${nspanel_mac}/log_backlog`, nspanel_backlog_callback);
    if (logs.length === 0) {
      const data = JSON.parse(message.body);
      setLogs(data.logs);
    }

    if (logs.length > maxLogs) {
      setLogs(logs.slice(0, maxLogs));
    }
  };

  useEffect(() => {
    if (settingsStatus === "none") {
      fetchSettingsData();
    }
    if (stompStatus == "none") {
      setHasSubscribed(false);
      useStompStore.getState().connect();
    }

    if (stompStatus == "connected" && !hasSubscribed) {
      useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log_backlog`, nspanel_backlog_callback);

      useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log`, (message: any) => {
        const log = JSON.parse(message.body) as INSpanelLogData;
        setLogs((prevLogs) => [log, ...prevLogs]);

        if (logs.length > maxLogs) {
          setLogs(logs.slice(0, maxLogs));
        }
      });

      setHasSubscribed(true);
    }
  }, [settingsStatus, fetchSettingsData, stompStatus, nspanel_mac]);

  const download_nspanel_logs_callback = (message) => {
    if (!hasDownloadedLogs) {
      const data = JSON.parse(message.body);
      const logs = [];
      data.logs.map((log: INSpanelLogData) => logs.push(`${log.time} ${log.level} ${log.message}`));
      const blob = new Blob([logs.join("\n")], { type: "text/plain" });
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = `nspanel_${nspanel_mac}_logs.txt`;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
      useStompStore.getState().unsubscribe(`nspanel/${nspanel_mac}/log_backlog`, download_nspanel_logs_callback);
      setHasDownloadedLogs(true);
    }
  };

  return (
    <div className="w-full bg-base-100 rounded-box p-3">
      <div className="flex items-center justify-between">
        <span className="text-lg font-semibold mb-1 block">Logs</span>
        <button
          className="btn btn-outline btn-sm btn-info"
          onClick={() => {
            setHasDownloadedLogs(false);
            useStompStore.getState().subscribe(`nspanel/${nspanel_mac}/log_backlog`, download_nspanel_logs_callback);
          }}
        >
          Download
        </button>
      </div>

      <div className="overflow-x-auto">
        <table className="table table-xs">
          <thead>
            <tr>
              <th className="px-3 py-1">Time</th>
              <th className="px-3 py-1">Level</th>
              <th className="px-3 py-1">Message</th>
            </tr>
          </thead>
          <tbody className="">
            {logs.map((log) => (
              <tr key={log} className="hover:bg-base-300">
                <td className="px-3 py-1">{log.time}</td>
                {(() => {
                  switch (log.level) {
                    case "TRACE":
                      return <td className="px-3 py-1 text-center bg-white text-black">{log.level}</td>;
                    case "DEBUG":
                      return <td className="px-3 py-1 text-center">{log.level}</td>;
                    case "INFO":
                      return <td className="px-3 py-1 bg-info text-info-content text-center">{log.level}</td>;
                    case "WARNING":
                      return <td className="px-3 py-1 bg-warning text-warning-content text-center">{log.level}</td>;
                    case "ERROR":
                      return <td className="px-3 py-1 bg-error text-error-content text-center">{log.level}</td>;
                    default:
                      return <td className="px-3 py-1">Unknown log level! {log.level}</td>;
                  }
                })()}
                <td className="px-3 py-1">{log.message}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}
