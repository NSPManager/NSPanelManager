import { getCookie } from "./client";

// Helpers for the older /rest/* endpoints the settings page uses to fill in and check connection
// settings. (InitialSetup has its own copies of these.) Each resolves to an error message, or null on success.

export type TestResult = string | null;

async function errorMessage(response: Response): Promise<string> {
  try {
    const data = await response.json();
    return data.message || data.error || `HTTP ${response.status}`;
  } catch {
    return `HTTP ${response.status}`;
  }
}

async function runTest(url: string, params: Record<string, string>): Promise<TestResult> {
  try {
    const response = await fetch(`${url}?${new URLSearchParams(params).toString()}`, { method: "GET", credentials: "same-origin" });
    return response.ok ? null : await errorMessage(response);
  } catch (error) {
    return error instanceof Error ? error.message : String(error);
  }
}

/** The manager's IP address for a hostname, as the server resolves it. */
export async function getIpByHostname(hostname: string): Promise<string> {
  const response = await fetch("/rest/get_ip_by_hostname", {
    method: "POST",
    credentials: "same-origin",
    headers: { "Content-Type": "application/json", "X-CSRFToken": getCookie("csrftoken") },
    body: JSON.stringify({ hostname }),
  });
  if (!response.ok) {
    throw new Error(await errorMessage(response));
  }
  const data = await response.json();
  if (!data.ip) {
    throw new Error('No "ip" in the response.');
  }
  return data.ip;
}

export function testManager(address: string, port: number | string): Promise<TestResult> {
  return runTest("/rest/mqttmanager/test", { address, port: String(port) });
}

export function testHomeAssistant(address: string, token: string): Promise<TestResult> {
  return runTest("/rest/home_assistant/test", { address, token });
}

export function testOpenhab(address: string, token: string): Promise<TestResult> {
  return runTest("/rest/openhab/test", { address, token });
}
