export {};

declare global {
  const createModule: (options?: Record<string, unknown>) => Promise<any>;
}
