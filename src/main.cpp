
#include "imgui.h"
#include <CashewLib/Application.h>
#include <CashewLib/EntryPoint.h>

#include <CashewLib/Image.h>
#include <cstdint>
#include <iomanip>

#include <CashewLib/Input/Input.h>
#include <CashewLib/Input/KeyCodes.h>
#include <memory>

class ExampleLayer : public Cashew::Layer {
public:
  virtual void onUIRender() override {
    ImGui::Begin("Configuration");
    if (ImGui::Button("Render") || ImGui::ColorEdit4("Temporary Colour: ", &m_tempColour.x)) {
      Render();
    }
    ImGui::Text("Viewport size: %d x %d", m_viewportWidth, m_viewportHeight);

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    m_viewportWidth = ImGui::GetContentRegionAvail().x;
    m_viewportHeight = ImGui::GetContentRegionAvail().y;

    if (m_image) {
      ImGui::Image(m_image->getDescriptorSet(),
                   {(float)m_image->getWidth(), (float)m_image->getHeight()},
                   ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  void Render() {
    if (!m_image || m_viewportWidth != m_image->getWidth() ||
        m_viewportHeight != m_image->getHeight()) {
      m_image = std::make_shared<Cashew::Image>(
          m_viewportWidth, m_viewportHeight, Cashew::ImageFormat::RGBA);

      delete[] m_imageData;
      m_imageData = new uint32_t[m_viewportWidth * m_viewportHeight];
    }

    for (uint32_t y = 0; y < m_viewportHeight; y++) {
      for (uint32_t x = 0; x < m_viewportWidth; x++) {
        m_imageData[x + y * m_viewportWidth] =
            ImGui::ColorConvertFloat4ToU32(m_tempColour);
      }
    }

    m_image->setData(m_imageData);
  }

private:
  std::shared_ptr<Cashew::Image> m_image;
  uint32_t *m_imageData = nullptr;
  uint32_t m_viewportWidth = 0;
  uint32_t m_viewportHeight = 0;
  ImVec4 m_tempColour;
};

Cashew::Application *Cashew::CreateApplication(int argc, char **argv) {
  Cashew::ApplicationSpecification spec;
  spec.Name = "Cashew Example";

  Cashew::Application *app = new Cashew::Application(spec);
  app->PushLayer<ExampleLayer>();
  app->SetMenubarCallback([app]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}
